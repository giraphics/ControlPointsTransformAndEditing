#include "Gfx3DTransformation.h"
#include <QOpenGLFunctions>

extern QOpenGLFunctions* g_OpenGLFunctions;

#define MAX_CHAR_STR 64

Gfx3DTransformation::Gfx3DTransformation()
{
    Initialize();
}

void Gfx3DTransformation::Initialize()
{
    memset(&m_TransformMemData, 0, sizeof(TransformData));

    SetMatrixMode(PROJECTION_MATRIX);
    LoadIdentity();

    SetMatrixMode(MODEL_MATRIX);
    LoadIdentity();

    SetMatrixMode(VIEW_MATRIX);
    LoadIdentity();

//    Error();
}

void Gfx3DTransformation::Error()
{
    unsigned int error;

    while((error = g_OpenGLFunctions->glGetError()) != GL_NO_ERROR)
    {
        char str[MAX_CHAR_STR] = {""};

        switch (error)
        {
            case GL_INVALID_ENUM:
            {
                strcpy(str, "GL_INVALID_ENUM");
                break;
            }

            case GL_INVALID_VALUE:
            {
                strcpy(str, "GL_INVALID_VALUE");
                break;
            }

            case GL_INVALID_OPERATION:
            {
                strcpy(str, "GL_INVALID_OPERATION");
                break;
            }

            case GL_OUT_OF_MEMORY:
            {
                strcpy(str, "GL_OUT_OF_MEMORY");
                break;
            }
        }

        printf("[ GL_ERROR ]\nERROR: %s\n", str );
    }
}

void Gfx3DTransformation::SetMatrixMode(MatrixType p_MatrixMode)
{
    m_TransformMemData.m_MatrixMode = p_MatrixMode;
}

void Gfx3DTransformation::LoadIdentity()
{
    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
            {
                GetModelMatrix().setToIdentity();
                break;
            }

        case VIEW_MATRIX:
            {
                GetViewMatrix().setToIdentity();
                break;
            }

        case PROJECTION_MATRIX:
            {
                GetProjectionMatrix().setToIdentity();
                break;
            }
    }
}

void Gfx3DTransformation::PushMatrix()
{
    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            if (m_TransformMemData.m_ModelMatrixIndex >= MAX_MODEL_MATRIX) return;

            // Copy by memory is preffered over assignment
            memcpy(&m_TransformMemData.m_ModelMatrix[ m_TransformMemData.m_ModelMatrixIndex + 1],
                   &m_TransformMemData.m_ModelMatrix[ m_TransformMemData.m_ModelMatrixIndex],
                   sizeof(QMatrix4x4));

            ++m_TransformMemData.m_ModelMatrixIndex;

            break;
        }

        case VIEW_MATRIX:
        {
            if (m_TransformMemData.m_ViewMatrixIndex >= MAX_VIEW_MATRIX) return;

            memcpy(&m_TransformMemData.m_ViewMatrix[m_TransformMemData.m_ViewMatrixIndex + 1],
                   &m_TransformMemData.m_ViewMatrix[m_TransformMemData.m_ViewMatrixIndex],
                   sizeof(QMatrix4x4));

            ++m_TransformMemData.m_ModelMatrixIndex;

            break;
        }

        case PROJECTION_MATRIX:
        {
            if (m_TransformMemData.m_ProjectionMatrixIndex >= MAX_PROJECTION_MATRIX) return;

            memcpy(&m_TransformMemData.m_ProjectionMatrix[ m_TransformMemData.m_ProjectionMatrixIndex + 1],
                   &m_TransformMemData.m_ProjectionMatrix[ m_TransformMemData.m_ProjectionMatrixIndex],
                   sizeof(QMatrix4x4));

            ++m_TransformMemData.m_ProjectionMatrixIndex;

            break;
        }
    }
}

void Gfx3DTransformation::PopMatrix()
{
    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            if (m_TransformMemData.m_ModelMatrixIndex == 0) return;

            --m_TransformMemData.m_ModelMatrixIndex;

            break;
        }

        case VIEW_MATRIX:
        {
            if (m_TransformMemData.m_ViewMatrixIndex == 0) return;

            --m_TransformMemData.m_ViewMatrixIndex;

            break;
        }

        case PROJECTION_MATRIX:
        {
            if (m_TransformMemData.m_ProjectionMatrixIndex == 0) return;

            --m_TransformMemData.m_ProjectionMatrixIndex;

            break;
        }
    }
}

void Gfx3DTransformation::LoadMatrix(QMatrix4x4& p_Matrix)
{
    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            memcpy(&GetModelMatrix(), &p_Matrix, sizeof(QMatrix4x4));

            break;
        }

        case VIEW_MATRIX:
        {
            memcpy(&GetViewMatrix(), &p_Matrix, sizeof(QMatrix4x4));

            break;
        }

        case PROJECTION_MATRIX:
        {
            memcpy(&GetProjectionMatrix(), &p_Matrix, sizeof(QMatrix4x4));

            break;
        }
    }
}

void Gfx3DTransformation::MultiplyMatrix(QMatrix4x4* p_Matrix)
{
    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix() *=  (*p_Matrix);

            break;
        }

        case VIEW_MATRIX:
        {
            GetViewMatrix() *=  (*p_Matrix);

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix() *=  (*p_Matrix);

            break;
        }
    }
}

void Gfx3DTransformation::Translate(float p_TranslateX, float p_TranslateY, float p_TranslateZ)
{
    QVector3D translationVector(p_TranslateX, p_TranslateY, p_TranslateZ);

    if (translationVector == QVector3D(0.0f, 0.0f, 0.0f)) return;

    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix().translate(translationVector);

            break;
        }

        case VIEW_MATRIX:
        {
            GetViewMatrix().translate(translationVector);

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix().translate(translationVector);

            break;
        }
    }
}

void Gfx3DTransformation::Rotate(float p_Angle, float p_RotateX, float p_RotateY, float p_RotateZ)
{
    if (p_Angle == 0) return;

    QVector3D rotationVector(p_RotateX, p_RotateY, p_RotateZ);

    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix().rotate(p_Angle, rotationVector);

            break;
        }

        case VIEW_MATRIX:
        {
            GetViewMatrix().rotate(p_Angle, rotationVector);

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix().rotate(p_Angle, rotationVector);

            break;
        }
    }
}

void Gfx3DTransformation::Scale(float p_ScaleX, float p_ScaleY, float p_ScaleZ)
{
    QVector3D scaleVector(p_ScaleX, p_ScaleY, p_ScaleZ);

    if (scaleVector == QVector3D(1.0f, 1.0f, 1.0f)) return;


    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix().scale(scaleVector);

            break;
        }

        case VIEW_MATRIX:
        {
            GetModelMatrix().scale(scaleVector);

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix().scale(scaleVector);

            break;
        }
    }
}

QMatrix4x4& Gfx3DTransformation::GetModelMatrix()
{
    return m_TransformMemData.m_ModelMatrix[m_TransformMemData.m_ModelMatrixIndex];
}

QMatrix4x4& Gfx3DTransformation::GetViewMatrix()
{
    return m_TransformMemData.m_ViewMatrix[m_TransformMemData.m_ViewMatrixIndex];
}

QMatrix4x4& Gfx3DTransformation::GetProjectionMatrix()
{
    return m_TransformMemData.m_ProjectionMatrix[m_TransformMemData.m_ProjectionMatrixIndex];
}

QMatrix4x4& Gfx3DTransformation::GetModelViewProjectionMatrix()
{
    m_TransformMemData.m_ModelViewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix() * GetModelMatrix();

    return m_TransformMemData.m_ModelViewProjectionMatrix;
}

QMatrix4x4& Gfx3DTransformation::GetModelViewMatrix()
{
    m_TransformMemData.m_ModelViewMatrix = GetViewMatrix() * GetModelMatrix();

    return m_TransformMemData.m_ModelViewMatrix;
}

QMatrix3x3 Gfx3DTransformation::MVNormalMatrix()
{
    return GetModelViewMatrix().normalMatrix();
}

void Gfx3DTransformation::OrthoView(float p_Left, float p_Right, float p_Bottom, float p_Top, float p_Near, float p_Far)
{
    QMatrix4x4 matrixOrtho;
    matrixOrtho.ortho(p_Left, p_Right, p_Bottom, p_Top, p_Near, p_Far);

    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix() = matrixOrtho;

            break;
        }

        case VIEW_MATRIX:
        {
            GetViewMatrix() = matrixOrtho;

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix() = matrixOrtho;

            break;
        }
    }
}

void Gfx3DTransformation::PerspectiveView(float p_FOV, float p_AspectRatio, float p_NearPlane, float p_FarPlane)
{
    QMatrix4x4 matrixPerspective;
    matrixPerspective.perspective(p_FOV, p_AspectRatio, p_NearPlane, p_FarPlane);

    switch (m_TransformMemData.m_MatrixMode)
    {
        case MODEL_MATRIX:
        {
            GetModelMatrix() = matrixPerspective;

            break;
        }

        case VIEW_MATRIX:
        {
            GetViewMatrix() = matrixPerspective;

            break;
        }

        case PROJECTION_MATRIX:
        {
            GetProjectionMatrix() = matrixPerspective;

            break;
        }
    }
}

void Gfx3DTransformation::LookAt(const QVector3D& p_Eye, const QVector3D& p_Center, const QVector3D& p_Up)
{
    QMatrix4x4 lookat;
    lookat.lookAt(p_Eye, p_Center, p_Up);

    MultiplyMatrix(&lookat);
}

QVector3D Gfx3DTransformation::Project(float p_ObjX, float p_ObjY, float p_ObjZ, const QMatrix4x4& p_ModelViewMatrix, const QMatrix4x4& p_ProjectionMatrix, const QVector4D& p_Viewport)
{
    QVector4D inObjectCoord(p_ObjX, p_ObjY, p_ObjZ, 1.0f);

    QVector4D modelViewCoord = p_ModelViewMatrix * inObjectCoord;

    inObjectCoord = p_ProjectionMatrix * modelViewCoord; // doubt - check computation?

    if (!inObjectCoord.w()) return QVector3D(0.0f, 0.0f, 0.0f);

    inObjectCoord.setX(inObjectCoord.x() / inObjectCoord.w());
    inObjectCoord.setY(inObjectCoord.y() / inObjectCoord.w());
    inObjectCoord.setZ(inObjectCoord.z() / inObjectCoord.w());

    inObjectCoord.setX(inObjectCoord.x() * 0.5f + 0.5f);
    inObjectCoord.setY(inObjectCoord.y() * 0.5f + 0.5f);
    inObjectCoord.setZ(inObjectCoord.z() * 0.5f + 0.5f);

    inObjectCoord.setX((inObjectCoord.x() * p_Viewport.z()) + p_Viewport.x());
    inObjectCoord.setY((inObjectCoord.y() * p_Viewport.w()) + p_Viewport.y());

    return QVector3D(inObjectCoord.x(), inObjectCoord.y(), inObjectCoord.z());
}

QVector3D Gfx3DTransformation::Unproject(float p_WinX, float p_WinY, float p_WinZ, const QMatrix4x4& p_ModelViewMatrix, const QMatrix4x4& p_ProjectionMatrix, const QVector4D& p_Viewport)
{
    QVector4D inWindowCoord(p_WinX, p_WinY, p_WinZ, 1.0f);
    QMatrix4x4 MVP;

    QVector4D outObjectCoord;

    MVP = p_ProjectionMatrix * p_ModelViewMatrix;

    QMatrix4x4 matrixInverse = MVP.inverted();

    inWindowCoord.setX((inWindowCoord.x() - p_Viewport.x()) / p_Viewport.z());
    inWindowCoord.setY((inWindowCoord.y() - p_Viewport.y()) / p_Viewport.w());

    inWindowCoord.setX((inWindowCoord.x() * 2.0f) - 1.0f);
    inWindowCoord.setY((inWindowCoord.y() * 2.0f) - 1.0f);
    inWindowCoord.setZ((inWindowCoord.z() * 2.0f) - 1.0f);

    outObjectCoord = matrixInverse * inWindowCoord;

    if (!outObjectCoord.w()) return QVector3D(0.0f, 0.0f, 0.0f);

    inWindowCoord.setX(outObjectCoord.x() / outObjectCoord.w());
    inWindowCoord.setY(outObjectCoord.y() / outObjectCoord.w());
    inWindowCoord.setZ(outObjectCoord.z() / outObjectCoord.w());

    return QVector3D(outObjectCoord.x(), outObjectCoord.y(), outObjectCoord.z());
}
