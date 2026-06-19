#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    MeshComponent::MeshComponent(GameObject* owner) : Component(owner)
    {

    }

    MeshComponent::~MeshComponent()
    {

    }

    void MeshComponent::extract(gfx::FrameRenderData& frame) const
    { 
        auto const* t = getGameObject()->getComponent<TransformComponent>();
        const math::Mat4 model = t ? t->getWorldMatrix() : math::Mat4::identity();
        frame.items.emplace_back(mesh_, model, material_);
    }
}