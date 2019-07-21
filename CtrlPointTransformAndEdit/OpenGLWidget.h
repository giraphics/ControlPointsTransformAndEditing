#ifndef GLMAINWIDGET_H
#define GLMAINWIDGET_H
#include<QOpenGLFunctions>
#include<QOpenGLWidget>
#include<QOpenGLFramebufferObject>

#include "TransformEditController.h"
#include "GraphicsCommon/Gfx3DShape.h"

#pragma pack(push, 1)
typedef struct
{
    int m_FrontBack;
    int m_PanY;
    int m_PanX;
//    float m_AnchorX;
//    float m_AnchorY;
    float m_Rotate;
    int m_Divergence;
    float m_ZoomX;
    float m_ZoomY;
    int NumOfChannels;
} ControlParameters;
#pragma pack(pop)

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWidget(QWidget* p_Parent = NULL);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int p_Width, int p_Height);
    virtual void paintGL();

    virtual void mousePressEvent(QMouseEvent* p_Event);
    virtual void mouseMoveEvent(QMouseEvent* p_Event);
    virtual void mouseReleaseEvent(QMouseEvent* p_Event);

    void ResetControlParams();

private:
    void SetUpEditTransform();

    GL3DCircle* m_Origin;
    Gfx3DScene* m_Scene;

    QColor m_SapphireBlue;
    QColor m_ShadowGray;

    GLint m_DefaultBuffer;
    qreal m_CachedDevicePixelRatio;
    ControlParameters m_ControlParams;

    bool m_IsMousePressed;

public:
    ControlParameters* GetControlParams() { return &m_ControlParams; }
    EditTransform* m_TransformWindow;
    EditTransformPosition* m_TransformPosition;
};

#endif // GLMAINWIDGET_H
