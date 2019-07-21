#include "Gfx3DProgramManager.h"

GfxGL3DProgramManager* GfxGL3DProgramManager::m_Instance(NULL);
QMutex GfxGL3DProgramManager::m_InstanceMutex;

GfxGL3DProgramManager::GfxGL3DProgramManager()
{
}

bool GfxGL3DProgramManager::AddShader(QOpenGLShaderProgram* p_Program, const QOpenGLShader::ShaderTypeBit& p_Type, const QString& p_Source)
{
    if (p_Source.isEmpty()) return false;

    if (!p_Program->addShaderFromSourceCode(p_Type, p_Source))
    {
        return false;
    }

    return true;
}

QOpenGLShaderProgram* GfxGL3DProgramManager::ProgramLoad(const QString& p_Name, const QString& p_VertexShader, const QString& p_FragmentShader)
{
    QOpenGLShaderProgram* program = new QOpenGLShaderProgram(this);

    if ((!AddShader(program, QOpenGLShader::Vertex, p_VertexShader)) ||
        (!AddShader(program, QOpenGLShader::Fragment, p_FragmentShader)))
    {
        delete program;
        return NULL;
    }

    if (program->link())
    {
        this->m_MapShader[p_Name] = program;
        return program;
    }
    else
    {
        delete program;
        return NULL;
    }
}

GfxGL3DProgramManager* GfxGL3DProgramManager::Singleton()
{
    if (!m_Instance)
    {
        QMutexLocker instanceMutexLocker(&m_InstanceMutex);
        if (!m_Instance)
        {
            m_Instance = new GfxGL3DProgramManager();
        }
    }

    return m_Instance;
}
