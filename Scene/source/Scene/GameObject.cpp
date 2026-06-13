#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>

namespace chai::scene
{
    GameObject::GameObject()
    {
        addComponent<TransformComponent>();
    }

    GameObject::GameObject(const std::string& name) : name_(name)
    {
        addComponent<TransformComponent>();
    }

    void GameObject::setParent(GameObject* parent)
    {
        parent_ = parent;
    }

    GameObject* GameObject::getParent() const
    {
        return parent_;
    }

    //std::span<GameObject const*>& GameObject::getChildren() const
    //{
    //    return m_children;
    //}

    void GameObject::update(float deltaTime)
    {
        for (const auto& component : components_) {
            if (auto updatable = dynamic_cast<IUpdatable*>(component.get())) {
                updatable->update(deltaTime);
            }
        }

        if (controllerComponent_) {
            controllerComponent_->update(deltaTime);
        }
    }

    void GameObject::extract(gfx::FrameRenderData& frame) const
    {
        for (const auto& c : components_)
            if (auto updatable = dynamic_cast<IUpdatable*>(c.get()))
                updatable->extract(frame);
        for (auto* child : children_)
            child->extract(frame);
    }
} // namespace chai::cup