#include "Gfx3DShape.h"
#include <QtGui/QOpenGLFunctions>
#include <math.h> // For sine, cosine

#include "GraphicsCommon/Gfx3DProgramManager.h"

extern QOpenGLFunctions* g_OpenGLFunctions;

Gfx3DShape::Gfx3DShape(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent, const QString& p_Name)
    : Gfx3DRenderableItem(p_Scene, p_Parent, p_Name),
      m_PositionBuffer(NULL),
      m_TextureBuffer(NULL)
{
}

Gfx3DShape::~Gfx3DShape()
{
    if (m_PositionBuffer)
    {
        m_PositionBuffer->destroy();
        delete m_PositionBuffer;
        m_PositionBuffer = NULL;
    }

    if (m_TextureBuffer)
    {
        m_TextureBuffer->destroy();
        delete m_TextureBuffer;
        m_TextureBuffer = NULL;
    }
}

static const char* vertexCircle =
"#version 120 \n"
"attribute highp vec4 posAttr; \n"
"attribute highp vec2 texAttr; \n"
"uniform highp mat4 model; \n"
"uniform highp mat4 view; \n"
"uniform highp mat4 projection; \n"
"varying vec2 texCoord; \n"
"void main() \n"
"{ \n"
"   texCoord = texAttr; \n"
"   mat4 _model = model; _model[0][0] = 1.0; _model[1][1] = 1.0; \n"
"   gl_Position = projection * view * model * posAttr; \n"
"} \n";

static const char* fragmentFillCircle =
"#version 120 \n"
"uniform lowp vec4 colUniform; \n"
"varying vec2 texCoord; \n"
"uniform highp float spread; \n"
"void main() \n"
"{ \n"
"   gl_FragColor  = colUniform;/*vec4(col.r,col.g,col.b, 1.0);*/ \n"
"   float weight = 0.0f; \n"
"   float weight2 = 0.0f; \n"
"   float dx     = texCoord.x - 0.5; \n"
"   float dy     = texCoord.y - 0.5; \n"
"   float length = sqrt(dx * dx + dy * dy); \n"
"   weight = smoothstep(0.3 * .95, 0.5, length); \n"
"   weight2 = smoothstep(spread * .5, spread, length); \n"
"   vec4 colorSpread = vec4(1.0, 0.0, 0.0, 1.0);  if (length < spread) { gl_FragColor = /*colorSpread*/mix(vec4(colorSpread.r, colorSpread.g, colorSpread.b, 1.0), vec4(colorSpread.r, colorSpread.g, colorSpread.b, 0.0), weight2); return; }\n"
"   /*colorSpread = mix(vec4(colorSpread.r, colorSpread.g, colorSpread.b, 1.0), vec4(colorSpread.r, colorSpread.g, colorSpread.b, 0.0), spread);*/ \n"
"   gl_FragColor  = mix(vec4(colUniform.r, colUniform.g, colUniform.b, 1.0), vec4(colUniform.r, colUniform.g, colUniform.b, 0.0), weight); \n"
"} \n";

static const char* fragmentBorderCircle =
"#version 120 \n"
"uniform lowp vec4 colUniform; \n"
"varying vec2 texCoord; \n"
"float smootherstep(float edge0, float edge1, float x) \n"
"{ \n"
"    // Scale, and clamp x to 0..1 range \n"
"    x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0); \n"
"    // Evaluate polynomial \n"
"    return x*x*(x*(x*6 - 15) + 10); \n"
"} \n"
" \n"
"float clampp(float x, float lowerlimit, float upperlimit) \n"
"{ \n"
"    if (x < lowerlimit) x = lowerlimit; \n"
"    if (x > upperlimit) x = upperlimit; \n"
"    return x; \n"
"} \n"
"void main() \n"
"{ \n"
"   gl_FragColor  = colUniform;/*vec4(col.r,col.g,col.b, 1.0);*/ \n"
"   float weight = 0.0f; \n"
"   float dx     = texCoord.x - 0.5; \n"
"   float dy     = texCoord.y - 0.5; \n"
"   float length = sqrt(dx * dx + dy * dy); \n"
"   weight = smoothstep(0.3, 0.5, length); \n"
"   float weight1 = smoothstep(0.18, 0.20, length); \n"
"   float weight2 = smoothstep(0.224, 0.24, length); \n"
"   weight = weight1 * (1.0 - weight2); \n"
"   gl_FragColor = mix( vec4(colUniform.r, colUniform.g, colUniform.b, 1.0), vec4(colUniform.g, colUniform.r, colUniform.b, 0.0), 1.0 - weight); \n"
"} \n";

GL3DCircle::GL3DCircle(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent /*= NULL*/, const QString& p_Name /*= ""*/, bool p_IsFilled /*= false*/, GLfloat p_LineWidth /*= 1*/)
    : Gfx3DShape(p_Scene, p_Parent, p_Name),
      m_IsFilled(p_IsFilled),
      m_LineWidth(p_LineWidth),
      m_Diameter(1.0f),
      m_Spread(0.5f)
{
}

GL3DCircle::~GL3DCircle()
{
    DestroyGeometry();
}

void GL3DCircle::Initialize()
{
    BuildGeometry();

    m_Program = GfxGL3DProgramManager::Singleton()->ProgramLoad("Circle", vertexCircle, fragmentFillCircle);
    if (!m_Program)
    {
        DestroyGeometry();
    }

    m_PosAttr = m_Program->attributeLocation("posAttr");
    m_TexAttr = m_Program->attributeLocation("texAttr");

    m_MatrixModel = m_Program->uniformLocation("model");
    m_MatrixView = m_Program->uniformLocation("view");
    m_MatrixProjection = m_Program->uniformLocation("projection");
    m_ColAttr = m_Program->uniformLocation("colUniform");
    m_SpreadAttr = m_Program->uniformLocation("spread");

    Gfx3DShape::Initialize();
}

void GL3DCircle::Render()
{
//    glEnable(GL_MULTISAMPLE);
    m_Scene->Transform().PushMatrix();
    ApplyTransformation();

    m_Program = GfxGL3DProgramManager::Singleton()->Program("Circle");
    m_Program->bind();
    m_Program->setUniformValue(m_MatrixProjection, m_Scene->Transform().GetProjectionMatrix());

    m_Program->setUniformValue(m_MatrixView, m_Scene->Transform().GetViewMatrix());
    m_Program->setUniformValue(m_ColAttr, QVector4D(m_Color.redF(), m_Color.greenF(), m_Color.blueF(), m_Color.alphaF()));
    m_Program->setUniformValue(m_SpreadAttr, m_Spread);
    m_Program->setUniformValue(m_MatrixModel, m_Scene->Transform().GetModelMatrix());
    m_PositionBuffer->bind();
    g_OpenGLFunctions->glVertexAttribPointer(m_PosAttr, 2, GL_FLOAT, GL_FALSE, 0, 0);

    m_TextureBuffer->bind();
    g_OpenGLFunctions->glVertexAttribPointer(m_TexAttr, 2, GL_FLOAT, GL_FALSE, 0, 0);
    g_OpenGLFunctions->glEnableVertexAttribArray(m_PosAttr);

    g_OpenGLFunctions->glLineWidth(m_LineWidth);
    g_OpenGLFunctions->glEnableVertexAttribArray(m_PosAttr);
    g_OpenGLFunctions->glEnableVertexAttribArray(m_TexAttr);
    g_OpenGLFunctions->glDrawArrays(m_IsFilled? GL_TRIANGLE_STRIP : GL_LINE_STRIP, 0, (m_PositionBuffer->size() / sizeof(GLfloat)) / 2);
    g_OpenGLFunctions->glEnableVertexAttribArray(m_TexAttr);
    g_OpenGLFunctions->glDisableVertexAttribArray(m_PosAttr);
    g_OpenGLFunctions->glLineWidth(1);

    Gfx3DShape::Render();
    m_Scene->Transform().PopMatrix();

    m_PositionBuffer->release();
    m_TextureBuffer->release();
}

void GL3DCircle::SetFilled(bool p_IsFilled)
{
    m_IsFilled = p_IsFilled;
    BuildGeometry();
}

void GL3DCircle::BuildGeometry()
{
    if (m_PositionBuffer == NULL)
    {
        m_PositionBuffer = new QOpenGLBuffer;
        m_PositionBuffer->create();
    }

    float radius = m_Diameter * 0.5f;
    if (m_PositionBuffer->bind())
    {
        GLfloat quad[] = {
                            -radius, -radius,
                             radius, -radius,
                            -radius,  radius,
                             radius,  radius,
                         };

        m_PositionBuffer->allocate(quad, sizeof(quad));
        m_PositionBuffer->release();
    }

    if (m_TextureBuffer == NULL)
    {
        m_TextureBuffer = new QOpenGLBuffer;
        m_TextureBuffer->create();
    }

    const float texCoords[8] = {  0.0f, 0.0f,
                                  1.0f, 0.0f,
                                  0.0f, 1.0f,
                                  1.0f, 1.0f
                               };

    if (m_TextureBuffer->bind())
    {
        m_TextureBuffer->allocate(texCoords, sizeof(texCoords));
        m_TextureBuffer->release();
    }
}

void GL3DCircle::DestroyGeometry()
{
    if (m_PositionBuffer)
    {
        m_PositionBuffer->destroy();
        delete m_PositionBuffer;
        m_PositionBuffer = NULL;
    }

    if (m_TextureBuffer)
    {
        m_TextureBuffer->destroy();
        delete m_TextureBuffer;
        m_TextureBuffer = NULL;
    }
}
