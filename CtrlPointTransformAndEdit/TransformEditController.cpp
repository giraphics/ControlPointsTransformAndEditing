#include "TransformEditController.h"
#include <QOpenGLFunctions>
#include "OpenGLWidget.h"
#include "GraphicsCommon/Gfx3DProgramManager.h"

#include "math.h"
//extern ControlParameters* controlParams;

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 1080

extern QOpenGLFunctions* g_OpenGLFunctions;
//extern ControlParameters* controlParams;
GL3DCircle* EditTransform::s_ControlPoint = NULL;

EditTransform::EditTransform(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent, const QString& p_Name)
    : Gfx3DShape(p_Scene, p_Parent, p_Name),
      m_Program(0),
      statcItem(NULL),
      m_SelectedControlPoint(NULL)
{
    controlPointList.clear();
}

EditTransform::~EditTransform()
{
    DestroyGeometry();
}

void EditTransform::Initialize()
{
    statcItem = new GL3DCircle(m_Scene, NULL, "BezierPathPoint", true, 1);
    statcItem->SetDiameter(25);
    statcItem->SetColor(QColor(125, 230, 150));
    statcItem->Initialize();

    s_ControlPoint = new GL3DCircle(m_Scene, this, "BezierControlPoint", true, 1);
    s_ControlPoint->SetDiameter(30);
    s_ControlPoint->SetColor(QColor(225, 230, 150));
//    s_ControlPoint->Initialize();

    float bottom    = -IMAGE_HEIGHT * 0.5f;
    float top       =  IMAGE_HEIGHT * 0.5f;
    float left      = -IMAGE_WIDTH  * 0.5f;
    float right     =  IMAGE_WIDTH  * 0.5f;

    m_BottomLeft    = AddControlPoint(left, bottom);
    m_BottomRight   = AddControlPoint(right, bottom);
    m_TopLeft       = AddControlPoint(left, top);
    m_TopRight      = AddControlPoint(right, top);

    m_LeftMiddle    = AddControlPoint(left, (top + bottom) * 0.5f);
    m_RightMiddle   = AddControlPoint(right, (top + bottom) * 0.5f);
    m_TopMiddle     = AddControlPoint((left + right) * 0.5f, top);
    m_BottomMiddle  = AddControlPoint((left + right) * 0.5f, bottom);

    m_AnchorPoint   = AddControlPoint((top + bottom) * 0.5f,  (left + right) * 0.5f);

    // Todo: release this circle item on destroy
    Gfx3DShape::Initialize();
}

void EditTransform::Render()
{
    m_Scene->Transform().PushMatrix();
    ApplyTransformation();

    DrawControlPoints();

    Gfx3DShape::Render();
    m_Scene->Transform().PopMatrix();
}

void EditTransform::BuildGeometry()
{
}

void EditTransform::DestroyGeometry()
{
}


void EditTransform::DrawControlPoints()
{
    for (int i = 0; i < controlPointList.size(); ++i)
    {
        ControlPoint* currentCntrlPnt = controlPointList.at(i);

        AnimateControlPoints(currentCntrlPnt);

        s_ControlPoint->Reset();
        s_ControlPoint->Translate(currentCntrlPnt->m_PosX, currentCntrlPnt->m_PosY, 0.0f);
        s_ControlPoint->Scale(1.0f / m_ContainerGlWidget->GetControlParams()->m_ZoomX, 1.0f / m_ContainerGlWidget->GetControlParams()->m_ZoomY, 1.0f);

        s_ControlPoint->SetSpread(currentCntrlPnt->radii * 0.5);
        s_ControlPoint->Render();
    }
}

void EditTransform::AnimateControlPoints(ControlPoint* p_CurrentControlPoint)
{
    if (!p_CurrentControlPoint) return;

    m_SelectedControlPoint == p_CurrentControlPoint ? s_ControlPoint->SetColor(QColor(255, 200, 120)) : s_ControlPoint->SetColor(QColor(34, 200, 255));

    if (p_CurrentControlPoint != m_SelectedControlPoint && p_CurrentControlPoint->radii != 0.0f)
    {
        if (p_CurrentControlPoint->radii < 0.0f)
            p_CurrentControlPoint->radii = 0.0f;
        else
            p_CurrentControlPoint->radii -= 0.01f;

        qDebug() << p_CurrentControlPoint->radii;
    }
    else if (m_SelectedControlPoint && m_SelectedControlPoint->radii < 1.0f)
    {
        qDebug() << m_SelectedControlPoint->radii;
        m_SelectedControlPoint->radii += 0.01f;
    }
}

void EditTransform::mousePress(int p_PosX, int p_PosY)
{
    int GlPosX;
    int GlPosY;
    m_SelectedControlPoint = GetSelectedPoint(p_PosX, p_PosY, GlPosX, GlPosY);

    QVector4D p_Viewport;
    p_Viewport.setX(0);
    p_Viewport.setY(0);
    p_Viewport.setZ(m_Scene->m_ScreenWidth);
    p_Viewport.setW(m_Scene->m_ScreenHeight);

    if (m_SelectedControlPoint == m_AnchorPoint)
    {
        QVector3D glPoint = m_Scene->Transform().Project(m_AnchorPoint->m_PosX, m_AnchorPoint->m_PosY, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
        m_GLMousePosAtClicked = QVector2D(glPoint.x(), glPoint.y());
    }
    else
    {
        m_ScreenMousePosAtClicked = QVector2D(p_PosX, p_PosY);

        QVector3D glPoint1 = m_Scene->Transform().Project(p_PosX, p_PosY, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
        m_GLMousePosAtClicked = QVector2D(glPoint1.x(), glPoint1.y());

        QVector3D glPoint2 = m_Scene->Transform().Project(p_PosX + 1.0f, p_PosY + 1.0f, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
        m_UnitPixelDisplacement = QVector2D(glPoint2.x(), glPoint2.y()) - m_GLMousePosAtClicked;
    }

    qDebug() << "m_GLMousePosAtClicked X" << m_GLMousePosAtClicked.x() <<"m_GLMousePosAtClicked" << m_GLMousePosAtClicked.y();

    if (!m_SelectedControlPoint)
    {
        float bottom    = -IMAGE_HEIGHT * 0.5f;
        float top       =  IMAGE_HEIGHT * 0.5f;
        float left      = -IMAGE_WIDTH  * 0.5f;
        float right     =  IMAGE_WIDTH  * 0.5f;

        if ((GlPosX <  left || GlPosX >  right) || (GlPosY >  top || GlPosY <  bottom))
        {
            qDebug() << "OutSide";
        }
        else
        {
            qDebug() << "INSide";
            m_InsideQuadClickedPosition.setX(GlPosX);
            m_InsideQuadClickedPosition.setY(GlPosY);
        }
    }
    else
    {
        //m_SelectedControlPoint->start();
    }

    return;
    //AddPoint(p_PosX, p_PosY);
}

void EditTransform::mouseHover(int p_PosX, int p_PosY)
{
    int GlPosX, GlPosY;
    m_SelectedControlPoint = GetSelectedPoint(p_PosX, p_PosY, GlPosX, GlPosY);
}

void EditTransform::mouseMove(int p_PosX, int p_PosY)
{
    QVector4D p_Viewport;
    p_Viewport.setX(0);
    p_Viewport.setY(0);
    p_Viewport.setZ(m_Scene->m_ScreenWidth);
    p_Viewport.setW(m_Scene->m_ScreenHeight);

    QVector3D glPoint = m_Scene->Transform().Unproject(p_PosX, p_PosY, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);

    if (!m_SelectedControlPoint)
    {
        float bottom    = -IMAGE_HEIGHT * 0.5f;
        float top       =  IMAGE_HEIGHT * 0.5f;
        float left      = -IMAGE_WIDTH  * 0.5f;
        float right     =  IMAGE_WIDTH  * 0.5f;

        if ((glPoint.x() <  left || glPoint.x() >  right) || (glPoint.y() >  top || glPoint.y() <  bottom))
        {
            qDebug() << "OutSide";
        }
        else
        {
            qDebug() << "INSide";

            //m_ContainerGlWidget->GetControlParams()->m_PanX += glPoint.x() - m_InsideQuadClickedPosition.x();
            //m_ContainerGlWidget->GetControlParams()->m_PanY += glPoint.y() - m_InsideQuadClickedPosition.y();
        }

//        m_ContainerGlWidget->GetControlParams()->m_PanX = (p_PosX - m_ScreenMousePosAtClicked.x()) * m_UnitPixelDisplacement.x();
//        m_ContainerGlWidget->GetControlParams()->m_PanY = (p_PosY - m_ScreenMousePosAtClicked.y()) * m_UnitPixelDisplacement.y();

        return;
    }

    QVector2D vectorSelectedToAnchor = QVector2D(m_AnchorPoint->m_PosX, m_AnchorPoint->m_PosY) - QVector2D (m_SelectedControlPoint->m_PosX, m_SelectedControlPoint->m_PosY);
    QVector2D vectorCurPositionToAnchor = QVector2D(m_AnchorPoint->m_PosX, m_AnchorPoint->m_PosY) - QVector2D (glPoint.x(), glPoint.y());
    QVector2D zoom = vectorCurPositionToAnchor / vectorSelectedToAnchor;

    // Apply Zoom to X and Y
    if (m_SelectedControlPoint == m_BottomLeft ||
        m_SelectedControlPoint == m_TopLeft ||
        m_SelectedControlPoint == m_BottomRight ||
        m_SelectedControlPoint == m_TopRight)
    {
        m_ContainerGlWidget->GetControlParams()->m_ZoomX = m_ContainerGlWidget->GetControlParams()->m_ZoomX * zoom.x();
        m_ContainerGlWidget->GetControlParams()->m_ZoomY = m_ContainerGlWidget->GetControlParams()->m_ZoomY * zoom.y();
    }
    else if (m_SelectedControlPoint == m_LeftMiddle || m_SelectedControlPoint == m_RightMiddle)
    {
        m_ContainerGlWidget->GetControlParams()->m_ZoomX = m_ContainerGlWidget->GetControlParams()->m_ZoomX * zoom.x();
    }
    else if (m_SelectedControlPoint == m_TopMiddle || m_SelectedControlPoint == m_BottomMiddle)
    {
        m_ContainerGlWidget->GetControlParams()->m_ZoomY = m_ContainerGlWidget->GetControlParams()->m_ZoomY * zoom.y();
    }
    else if (m_SelectedControlPoint == m_AnchorPoint)
    {
        bool useUnprojectedCoord = true;
        float angle  = 0.0f;
        QVector2D slopeVector;
        if (useUnprojectedCoord)
        {
            slopeVector = QVector2D(glPoint.x(), glPoint.y()) - QVector2D(m_AnchorPoint->m_PosX, m_AnchorPoint->m_PosY);
            angle = atan2(slopeVector.y(), slopeVector.x()) * 180 / 3.14;
            m_ContainerGlWidget->GetControlParams()->m_Rotate += angle;
        }
        else
        {
            slopeVector =  QVector2D(p_PosX, p_PosY) - m_GLMousePosAtClicked;
            static float lastAngle = 0.0f;
            angle = atan2(slopeVector.y(), slopeVector.x()) * 180 / 3.14;
            m_ContainerGlWidget->GetControlParams()->m_Rotate +=  angle - lastAngle;
            lastAngle = angle;
        }
        qDebug() << "angle: "<< angle;
    }

    // Don't let the zoom go 0, otherwise every all points will get concentrate at single point
    if (m_ContainerGlWidget->GetControlParams()->m_ZoomX < 0.03)
        m_ContainerGlWidget->GetControlParams()->m_ZoomX = 0.03;

    if (m_ContainerGlWidget->GetControlParams()->m_ZoomY < 0.03)
        m_ContainerGlWidget->GetControlParams()->m_ZoomY = 0.03;
    //AddPoint(p_PosX, p_PosY);
}

void EditTransform::mouseRelease(int p_PosX, int p_PosY)
{
}

ControlPoint* EditTransform::AddPoint(int p_PosX, int p_PosY)
{
    QVector4D p_Viewport;
    p_Viewport.setX(0);
    p_Viewport.setY(0);
    p_Viewport.setZ(m_Scene->m_ScreenWidth);
    p_Viewport.setW(m_Scene->m_ScreenHeight);

    QVector3D glPoint = m_Scene->Transform().Unproject(p_PosX, p_PosY, 0.0f, m_Scene->Transform().GetModelViewMatrix() * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
    ControlPoint* cp = new ControlPoint(/*m_Scene, NULL, "ControlPoint", true, 1*/);
    cp->m_PosX = glPoint.x();
    cp->m_PosY = glPoint.y();

    controlPointList << cp;
}

ControlPoint* EditTransform::AddControlPoint(int p_PosX, int p_PosY)
{
    ControlPoint* cp = new ControlPoint();
    cp->m_PosX = p_PosX;
    cp->m_PosY = p_PosY;

    controlPointList << cp;
    return cp;
}

ControlPoint* EditTransform::GetSelectedPoint(int p_PosX, int p_PosY, int& p_GlPosX, int& p_GlPosY)
{
    QVector4D p_Viewport;
    p_Viewport.setX(0);
    p_Viewport.setY(0);
    p_Viewport.setZ(m_Scene->m_ScreenWidth);
    p_Viewport.setW(m_Scene->m_ScreenHeight);

    QVector3D glPoint = m_Scene->Transform().Unproject(p_PosX, p_PosY, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
    QVector3D glPointPlus11 = m_Scene->Transform().Unproject(p_PosX + 1, p_PosY + 1, 0.0f, m_Scene->Transform().GetModelViewMatrix() * ((Gfx3DRenderableItem*)parent())->m_Transformation * m_Transformation, m_Scene->Transform().GetProjectionMatrix(), p_Viewport);
    QVector3D glUnitPixel = glPointPlus11 - glPoint;
    p_GlPosX = glPoint.x();
    p_GlPosY = glPoint.y();

    for (int i = 0; i < controlPointList.size(); ++i)
    {
        // Control point under the radius of 10 pixel
        if (fabs(controlPointList.at(i)->m_PosX - glPoint.x()) < fabs(glUnitPixel.x() * 10) &&
            fabs(controlPointList.at(i)->m_PosY - glPoint.y()) < fabs(glUnitPixel.y() * 10))
        {
            return controlPointList.at(i);
        }
    }

    return NULL;
}

void EditTransform::RemoveControlPoint(ControlPoint* p_ControlPoint)
{
    controlPointList.removeAll(p_ControlPoint);
}

void EditTransform::SetOpenGLWidget(OpenGLWidget* p_GlWidget)
{
    m_ContainerGlWidget = p_GlWidget;
}


EditTransformPosition::EditTransformPosition(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent, const QString& p_Name)
    : Gfx3DShape(p_Scene, p_Parent, p_Name),
      m_TransformEditPosition(NULL)
{
}

EditTransformPosition::~EditTransformPosition()
{
}

void EditTransformPosition::Initialize()
{
    m_TransformEditPosition = new GL3DCircle(m_Scene, NULL, "TransformEditPoistionCircle", true, 1);
    m_TransformEditPosition->SetDiameter(450);
    m_TransformEditPosition->SetColor(QColor(25, 230, 150));
    m_TransformEditPosition->Initialize();
    Gfx3DShape::Initialize();
}

void EditTransformPosition::Render()
{
    m_Scene->Transform().PushMatrix();
    ApplyTransformation();

    m_TransformEditPosition->Reset();
    m_TransformEditPosition->Translate(m_ContainerGlWidget->GetControlParams()->m_PanX, m_ContainerGlWidget->GetControlParams()->m_PanY, 0.0f);
    //m_TransformEditPosition->Render();

    Gfx3DShape::Render();
    m_Scene->Transform().PopMatrix();
}

void EditTransformPosition::SetOpenGLWidget(OpenGLWidget* p_GlWidget)
{
    m_ContainerGlWidget = p_GlWidget;
}
