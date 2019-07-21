#include "OpenGLWidget.h"
#include<QElapsedTimer>
#include<QMouseEvent>

#ifndef FAIRLIGHT_3DPAN
QOpenGLFunctions* g_OpenGLFunctions = NULL;
#else
static QOpenGLFunctions* g_OpenGLFunctions = NULL;
#endif

#define FPS_TIME_START { \
                            static QElapsedTimer FPS; static int m_Frame;\
                            if (!FPS.isValid()) { FPS.start(); }

#define FPS_TIME_END        ++m_Frame; \
                            const int threshold = 3 * 1000; \
                            const int timing = FPS.elapsed(); \
                            if (timing > threshold) \
                            { \
                                qDebug("FPS: %f", m_Frame / 3.0f);   qDebug("Children: %d", m_Origin->children().size()); \
                                m_Frame = 0; \
                                FPS.start(); \
                            }   \
                        }
ControlParameters* m_ContainerGlWidget = NULL;
OpenGLWidget::OpenGLWidget(QWidget* p_Parent/* = NULL*/)
    : QOpenGLWidget(p_Parent),
      m_Origin(NULL),
      m_TransformWindow(NULL),
      m_Scene(NULL),
      m_TransformPosition(NULL)
{
    m_SapphireBlue = QColor(15, 82, 186, 255);
    m_ShadowGray = QColor(55, 55, 55, 255);

    // Keep track of the mouse to make mouse hover
    setMouseTracking(true);
    m_IsMousePressed = false;

    ResetControlParams();
    m_ContainerGlWidget = &m_ControlParams;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    #ifndef FAIRLIGHT_3DPAN
    // Initiliaze the global pointer to suffice the reported link time errors for OpenGL functions
    g_OpenGLFunctions = context()->functions();
    #endif

    SetUpEditTransform();
}

void OpenGLWidget::resizeGL(int p_Width, int p_Height)
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, p_Width * retinaScale, p_Height * retinaScale);

    m_Scene->Resize(p_Width, p_Height);
}

void OpenGLWidget::paintGL()
{
    FPS_TIME_START

    const qreal retinaScale = devicePixelRatio();
    if (!qFuzzyCompare(retinaScale, m_CachedDevicePixelRatio))
    {
        m_CachedDevicePixelRatio = retinaScale;
    }

    glClearColor(m_ShadowGray.redF(), m_ShadowGray.greenF(), m_ShadowGray.blueF(), m_ShadowGray.alphaF());
    m_Scene->Render();

    FPS_TIME_END
    m_Origin->Reset();
    m_Origin->Translate(m_ControlParams.m_PanX, m_ControlParams.m_PanY, 0.0f);

    m_TransformWindow->Reset();
    m_TransformWindow->Translate(m_TransformWindow->m_AnchorPoint->m_PosX/* + m_ControlParams.m_PanX*/, m_TransformWindow->m_AnchorPoint->m_PosY/* + m_ControlParams.m_PanY*/, 0.0f);
    m_TransformWindow->Rotate(m_ControlParams.m_Rotate/1.0f, 0.0f, 0.0f, 1.0f);
    m_TransformWindow->Translate(-m_TransformWindow->m_AnchorPoint->m_PosX/* - m_ControlParams.m_PanX*/, -m_TransformWindow->m_AnchorPoint->m_PosY/* - m_ControlParams.m_PanY*/, 0.0f);

    m_TransformWindow->Translate(m_TransformWindow->m_AnchorPoint->m_PosX/* + m_ControlParams.m_PanX*/, m_TransformWindow->m_AnchorPoint->m_PosY/* + m_ControlParams.m_PanY*/, 0.0f);
    m_TransformWindow->Scale(m_ControlParams.m_ZoomX, m_ControlParams.m_ZoomY, 1.0f);
    m_TransformWindow->Translate(-m_TransformWindow->m_AnchorPoint->m_PosX/* - m_ControlParams.m_PanX*/, -m_TransformWindow->m_AnchorPoint->m_PosY/* - m_ControlParams.m_PanY*/, 0.0f);

//    m_TransformWindow->Translate(m_ControlParams.m_PanX, m_ControlParams.m_PanY, 0.0f);

    update();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* p_Event)
{
    m_TransformWindow->mousePress(p_Event->pos().x(), height() - p_Event->pos().y());
    m_IsMousePressed = true;
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* p_Event)
{
    if (m_IsMousePressed)
    {
        m_TransformWindow->mouseMove(p_Event->pos().x(), height() - p_Event->pos().y());
        return;
    }
    m_TransformWindow->mouseHover(p_Event->pos().x(), height() - p_Event->pos().y());
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_IsMousePressed = false;
}

void OpenGLWidget::ResetControlParams()
{
    m_ControlParams.m_FrontBack = 0.0f;
    m_ControlParams.m_PanX = 0.0f;
    m_ControlParams.m_PanY = 0.0f;
    m_ControlParams.m_Rotate = 0.0f;
    m_ControlParams.m_ZoomX = 1.0f;
    m_ControlParams.m_ZoomY = 1.0f;
    //m_ControlParams.m_Divergence = 1.0f;
    //m_ControlParams.NumOfChannels = 15;
}

void OpenGLWidget::SetUpEditTransform()
{
    m_Scene = new Gfx3DScene(this, "3DPan");
    g_OpenGLFunctions->glEnable(GL_BLEND);
    g_OpenGLFunctions->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    g_OpenGLFunctions->glEnable(GL_DEPTH_TEST);
    g_OpenGLFunctions->glDepthMask(GL_TRUE);
    g_OpenGLFunctions->glDepthFunc(GL_LEQUAL);
    g_OpenGLFunctions->glFrontFace(GL_CCW);

    m_Origin = new GL3DCircle(m_Scene, NULL, "Origin", true, 1);
    m_Origin->SetDiameter(10);
    m_Origin->SetColor(QColor(255, 255, 255));

    m_TransformPosition = new EditTransformPosition(m_Scene, NULL, "TransformEditPositionObject");
    m_TransformPosition->SetOpenGLWidget(this);

    m_TransformWindow = new EditTransform(m_Scene, m_TransformPosition, "TransformEditRectangleWindow");
    m_TransformWindow->SetOpenGLWidget(this);

    m_Scene->Initialize();
}
