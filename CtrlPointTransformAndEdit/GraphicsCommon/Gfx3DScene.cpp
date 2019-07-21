#include "Gfx3DScene.h"
#include "Gfx3DRenderableItem.h"
#include<QOpenGLFunctions>

extern QOpenGLFunctions* g_OpenGLFunctions;

Gfx3DScene::Gfx3DScene(QObject* p_GLWindow, const QString& p_Name)
    : QObject(p_GLWindow),
      m_Frame(0),
      m_ScreenWidth(800),
      m_ScreenHeight(600)
{
    assert(p_GLWindow);

    setObjectName(p_Name);
}

Gfx3DScene::~Gfx3DScene()
{
    foreach (Gfx3DRenderableItem* currentModel, m_ModelList)
    {
        delete currentModel;
    }
}

void Gfx3DScene::Initialize()
{
    foreach (Gfx3DRenderableItem* currentModel, m_ModelList)
    {
        currentModel->Initialize();
    }
}

void Gfx3DScene::Render()
{
    //g_OpenGLFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SetUpProjection();

    foreach (Gfx3DRenderableItem* currentModel, m_ModelList)
    {
        currentModel->Render();
    }
}

void Gfx3DScene::AddRenderItem(Gfx3DRenderableItem* p_Model)
{
    if (p_Model && !p_Model->parent())
    {
        m_ModelList.push_back(p_Model);
    }
}

void Gfx3DScene::RemoveRenderItem(Gfx3DRenderableItem* p_Model)
{
    m_ModelList.removeAll(p_Model);
}

void Gfx3DScene::Resize(int p_Width, int p_Height)
{
    m_ScreenWidth = p_Width;
    m_ScreenHeight = p_Height;
}

void Gfx3DScene::SetUpProjection()
{
    m_Transform.SetMatrixMode(Gfx3DTransformation::PROJECTION_MATRIX);
    m_Transform.LoadIdentity();

    //m_Transform.PerspectiveView(60.0f, float(m_ScreenWidth)/m_ScreenHeight, 0.10f, 100.0f);
    m_Transform.OrthoView(-m_ScreenWidth, m_ScreenWidth, -m_ScreenHeight, m_ScreenHeight, -10.0f, 10.0f);

    m_Transform.SetMatrixMode(Gfx3DTransformation::VIEW_MATRIX);
    m_Transform.LoadIdentity();
    //m_Transform.Translate(0.60f, -0.25f, -3.50f);
    m_Transform.Translate(0.0f, 0.0f, -2.5f);

    m_Transform.SetMatrixMode(Gfx3DTransformation::MODEL_MATRIX);
    m_Transform.LoadIdentity();
}
