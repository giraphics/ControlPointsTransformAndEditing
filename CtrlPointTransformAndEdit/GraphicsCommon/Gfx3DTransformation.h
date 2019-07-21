#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <QMatrix4x4>

#define MAX_PROJECTION_MATRIX   2
#define MAX_MODEL_MATRIX        8
#define MAX_VIEW_MATRIX         8

class Gfx3DTransformation
{
public:
    enum MatrixType
    {
        MODEL_MATRIX = 0,
        VIEW_MATRIX = 1,
        PROJECTION_MATRIX = 2,
    };

    typedef struct
    {
        MatrixType m_MatrixMode;

        uint8_t m_ModelMatrixIndex;
        uint8_t m_ViewMatrixIndex;
        uint8_t m_ProjectionMatrixIndex;

        QMatrix4x4 m_ModelMatrix[MAX_MODEL_MATRIX];
        QMatrix4x4 m_ViewMatrix[MAX_VIEW_MATRIX];
        QMatrix4x4 m_ProjectionMatrix[MAX_PROJECTION_MATRIX];

        QMatrix4x4 m_ModelViewProjectionMatrix;
        QMatrix4x4 m_ModelViewMatrix;
    }TransformData;

public:
    Gfx3DTransformation();

    void Initialize();
    void Error();

    void SetMatrixMode(MatrixType p_MatrixMode);
    void LoadIdentity();

    void PushMatrix();
    void PopMatrix();

    void LoadMatrix(QMatrix4x4& p_Matrix);
    void MultiplyMatrix(QMatrix4x4* p_Matrix);

    void Translate(float p_TranslateX, float p_TranslateY, float p_TranslateZ);
    void Rotate(float p_Angle, float p_RotateX, float p_RotateY, float p_RotateZ);
    void Scale(float p_ScaleX, float p_ScaleY, float p_ScaleZ);

    QMatrix4x4& GetModelMatrix();
    QMatrix4x4& GetViewMatrix();
    QMatrix4x4& GetProjectionMatrix();

    QMatrix4x4& GetModelViewProjectionMatrix();
    QMatrix4x4& GetModelViewMatrix();
    QMatrix3x3 MVNormalMatrix();

    void OrthoView(float p_Left, float p_Right, float p_Bottom, float p_Top, float p_Near, float p_Far);
    void PerspectiveView(float p_FOV, float p_AspectRatio, float p_NearPlane, float p_FarPlane);
    void LookAt(const QVector3D& p_Eye, const QVector3D& p_Center, const QVector3D& p_Up);

    QVector3D Project(float p_ObjX, float p_ObjY, float p_ObjZ, const QMatrix4x4& p_ModelViewMatrix, const QMatrix4x4& p_ProjectionMatrix, const QVector4D& p_Viewport);
    QVector3D Unproject(float p_WinX, float p_WinY, float p_WinZ, const QMatrix4x4& p_ModelViewMatrix, const QMatrix4x4& p_ProjectionMatrix, const QVector4D& p_Viewport);

private:
    TransformData m_TransformMemData;
};

#endif // TRANSFORMATION_H
