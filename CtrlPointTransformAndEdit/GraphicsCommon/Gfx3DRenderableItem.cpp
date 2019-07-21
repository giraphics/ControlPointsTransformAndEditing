#include "Gfx3DRenderableItem.h"

Gfx3DRenderableItem::Gfx3DRenderableItem(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent, const QString& p_Name)
    : m_Scene(p_Scene),
      QObject(p_Parent),
      m_Center(),
      m_IsVisible(true),
      m_Program(0)
{
    assert(p_Scene);

    p_Scene->AddRenderItem(this);

    setObjectName(p_Name);
}

void Gfx3DRenderableItem::Initialize()
{
    foreach (QObject* objectItem, children())
    {
        Gfx3DRenderableItem* model = dynamic_cast<Gfx3DRenderableItem*>(objectItem);

        if (!model) continue;

        model->Initialize();
    }
}

void Gfx3DRenderableItem::Render()
{
    if (!m_IsVisible) return;

    foreach (QObject* objectItem, children())
    {
        Gfx3DRenderableItem* model = dynamic_cast<Gfx3DRenderableItem*>(objectItem);

        if (!model || !model->m_IsVisible) continue;

        model->Render();
    }
}

void Gfx3DRenderableItem::Rotate(float p_Angle, float p_X, float p_Y, float p_Z)
{
    m_Transformation.translate(m_Center);
    m_Transformation.rotate(p_Angle, p_X, p_Y, p_Z);
    m_Transformation.translate(-m_Center);
}

void Gfx3DRenderableItem::Translate(float p_X, float p_Y, float p_Z)
{
    m_Transformation.translate(QVector3D(p_X, p_Y, p_Z));
}

void Gfx3DRenderableItem::Scale(float p_X, float p_Y, float p_Z)
{
    m_Transformation.scale(QVector3D(p_X, p_Y, p_Z));
}

void Gfx3DRenderableItem::Reset()
{
    m_Transformation.setToIdentity();
}

void Gfx3DRenderableItem::ApplyTransformation()
{
    m_Scene->Transform().GetModelMatrix() *= m_Transformation;
}
