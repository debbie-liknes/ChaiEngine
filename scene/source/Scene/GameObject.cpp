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

    GameObject::GameObject(const std::string& name, GameObjectId id) : name_(name), objectId_(id)
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

    void GameObject::update(const UpdateContext& ctx)
    {
        for (const auto& component : components_) {
            if (auto updatable = dynamic_cast<IUpdatable*>(component.get())) {
                updatable->update(ctx);
            }
        }

        if (controllerComponent_) {
            controllerComponent_->update(ctx);
        }
    }

    void GameObject::extract(gfx::FrameRenderData& frame) const
    {
        for (const auto& c : components_)
            if (auto updatable = dynamic_cast<IUpdatable*>(c.get()))
                updatable->extract(frame);
        for (auto const* child : children_)
        {
            if (child->parent_ == nullptr)
                child->extract(frame);
        }
    }
} // namespace chai::cup