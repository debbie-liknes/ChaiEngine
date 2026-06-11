#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    GameObject::GameObject()
    {
        addComponent<TransformComponent>();
    }

    GameObject::GameObject(const std::string& name) : m_name(name)
    {
        addComponent<TransformComponent>();
    }

    void GameObject::setParent(GameObject* parent)
    {
        m_parent = parent;
    }

    GameObject* GameObject::getParent() const
    {
        return m_parent;
    }

    //std::span<GameObject const*>& GameObject::getChildren() const
    //{
    //    return m_children;
    //}

    void GameObject::update(double deltaTime)
    {
        for (const auto& component : m_components) {
            if (auto updatable = dynamic_cast<IUpdatable*>(component.get())) {
                updatable->update(deltaTime);
            }
        }

        if (controllerComponent) {
            controllerComponent->update(deltaTime);
        }
    }

    void GameObject::extract(gfx::FrameRenderData& frame) const
    {
        for (const auto& c : m_components)
            c->extract(frame);
        for (auto* child : m_children)
            child->extract(frame);
    }
} // namespace chai::cup