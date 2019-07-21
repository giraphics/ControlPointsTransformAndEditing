#ifndef UIFL_RENDERABLE_ITEM_H
#define UIFL_RENDERABLE_ITEM_H

#include <QPointer>
#include <assert.h>

#include "GraphicsCommon/Gfx3DScene.h"

class Gfx3DRenderableItem : public QObject
{
    Q_OBJECT
public:
    Gfx3DRenderableItem(Gfx3DScene* p_Scene, Gfx3DRenderableItem* p_Parent, const QString& p_Name = "");
    virtual ~Gfx3DRenderableItem(){}

    virtual void Initialize() = 0;
    virtual void Render() = 0;

    void Rotate(float p_Angle, float p_X, float p_Y, float p_Z);
    void Translate(float p_X, float p_Y, float p_Z);
    void Scale(float p_X, float p_Y, float p_Z);
    void Reset();

    void SetCenter(const QVector3D& p_Center) { m_Center = p_Center; }
    QVector3D Center() const { return m_Center; }

    void SetVisible(bool p_IsVisible) { m_IsVisible = p_IsVisible; }
    bool IsVisible() const { return m_IsVisible; }

    void ApplyTransformation();
    QMatrix4x4 m_Transformation;

protected:
    virtual void BuildGeometry() {}
    virtual void DestroyGeometry() { printf("Renderable item destructor"); }

    unsigned int m_Program;
    Gfx3DScene* m_Scene;
    QVector3D m_Center;
    bool m_IsVisible;
    QPointer<Gfx3DRenderableItem> m_Parent;
};
#endif // ABSTRACTMODEL_H
