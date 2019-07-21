#ifndef GFX3DSHAPE_H
#define GFX3DSHAPE_H

#include <QOpenGLBuffer>
#include <QtGui/QOpenGLShaderProgram>
#include <QColor>

#include "GraphicsCommon/Gfx3DRenderableItem.h"

class Gfx3DShape : public Gfx3DRenderableItem
{

public:
    Gfx3DShape(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent = 0, const QString& p_Name = "");
    ~Gfx3DShape();

protected:
    GLuint m_PosAttr;
    GLuint m_TexAttr;

    GLuint m_MatrixModel;
    GLuint m_MatrixView;
    GLuint m_MatrixProjection;

    QOpenGLBuffer* m_PositionBuffer;
    QOpenGLBuffer* m_TextureBuffer;
    QOpenGLShaderProgram* m_Program;
};

class GL3DCircle : public Gfx3DShape
{
public:
    GL3DCircle(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent = NULL, const QString& p_Name = "", bool p_IsFilled = false, GLfloat p_LineWidth = 1);
    virtual ~GL3DCircle();

    void Initialize();
    void Render();

    void SetColor(const QColor& p_Color) { m_Color = p_Color; }
    void SetFilled(bool p_IsFilled = true);
    bool isFilled() const { return m_IsFilled; }

    void SetDiameter(float p_Diameter) { m_Diameter = p_Diameter; }
    float Diameter() const { return m_Diameter; }
    void SetSpread(float p_Spread) { m_Spread = p_Spread; }

private:
    void BuildGeometry();
    void DestroyGeometry();

    bool m_IsFilled;
    GLfloat m_LineWidth;
    GLuint m_ColAttr;
    GLuint m_SpreadAttr;
    QColor m_Color;
    float m_Diameter;
    float m_Spread;
};

#endif // GFX3DSHAPE_H
