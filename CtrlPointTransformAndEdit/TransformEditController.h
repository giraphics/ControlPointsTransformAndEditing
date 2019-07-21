#ifndef TRANSFORM_WINDOW_H
#define TRANSFORM_WINDOW_H
#include <QtGui/QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QList>
#include <QTimer>

#include "GraphicsCommon/Gfx3DShape.h"
class OpenGLWidget;

class ControlPoint : public QObject
{
    Q_OBJECT
public:
    ControlPoint()
    {
        timer = new QTimer(this);
        timer->setSingleShot(true);
        m_Duration = 10;
        connect(timer, SIGNAL(timeout()), this, SLOT(updateRadii()));
        radii = 0.0f;
    }

public:
    float m_PosX;
    float m_PosY;
    QTimer* timer;
    float radii;
    int m_Duration;

    void start() { timer->start(m_Duration); }

public slots:
    void updateRadii()
    {
        //radii = m_Duration - timer->remainingTime() / m_Duration;
        radii += 0.1;
        //qDebug() << "update..." << radii;
        if (radii < 1.0f)
        start();
    }
};

class EditTransformPosition : public Gfx3DShape
{

public:
    EditTransformPosition(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent = 0, const QString& p_Name = "");
    virtual ~EditTransformPosition();

    void Initialize();
    void Render();
    virtual void BuildGeometry(){}
    virtual void DestroyGeometry(){}
    void SetOpenGLWidget(OpenGLWidget* p_GlWidget);

private:
    OpenGLWidget* m_ContainerGlWidget;
    GL3DCircle* m_TransformEditPosition;
};

class EditTransform : public Gfx3DShape
{
public:
    EditTransform(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent = 0, const QString& p_Name = "");
    virtual ~EditTransform();

    void Initialize();
    void Render();
    virtual void BuildGeometry();
    virtual void DestroyGeometry();

    void mousePress(int p_PosX, int p_PosY);
    void mouseMove(int p_PosX, int p_PosY);
    void mouseHover(int p_PosX, int p_PosY);
    void mouseRelease(int p_PosX, int p_PosY);

    ControlPoint* AddPoint(int p_PosX, int p_PosY);
    ControlPoint* AddControlPoint(int p_PosX, int p_PosY);
    ControlPoint* GetSelectedPoint(int p_PosX, int p_PosY, int& p_GlPosX, int& p_GlPosY);

    void RemoveControlPoint(ControlPoint* p_ControlPoint);

    void SetOpenGLWidget(OpenGLWidget* p_GlWidget);

private:
    QOpenGLShaderProgram* m_Program;
    OpenGLWidget* m_ContainerGlWidget;
    QList<ControlPoint*> controlPointList; // Need to change it to list
    GL3DCircle* statcItem;
    static GL3DCircle* s_ControlPoint;
    ControlPoint* m_SelectedControlPoint;
    QPointF m_InsideQuadClickedPosition;
    QVector2D m_GLMousePosAtClicked;
    QVector2D m_ScreenMousePosAtClicked;
    QVector2D m_UnitPixelDisplacement;

public:
    ControlPoint* m_BottomLeft;
    ControlPoint* m_BottomRight;
    ControlPoint* m_TopLeft;
    ControlPoint* m_TopRight;

    ControlPoint* m_LeftMiddle;
    ControlPoint* m_RightMiddle;
    ControlPoint* m_TopMiddle;
    ControlPoint* m_BottomMiddle;

    ControlPoint* m_AnchorPoint;

private:
    void DrawControlPoints();
    void AnimateControlPoints(ControlPoint* p_CurrentControlPoint);
};

#endif // TRANSFORM_WINDOW_H
