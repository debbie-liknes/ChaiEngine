#include <Components/MeshComponent.h>
#include <Components/TransformComponent.h>
#include <Scene/GameObject.h>
#include <Visitors/Visitor.h>

namespace chai::scene
{
    GameObject::GameObject() : Object()
    {
        addComponent<TransformComponent>();
    }

    GameObject::GameObject(const std::string& name) : name_(name)
    {
        addComponent<TransformComponent>();
    }

    void GameObject::accept(Visitor* visitor)
    {
        visitor->visit(this);

        for (const auto& c : components_)
            c->accept(visitor);
        for (auto* child : children_)
            if (child->parent_ == nullptr)
                child->accept(visitor);
    }

    void GameObject::setParent(GameObject* parent)
    {
        if (parent_)
            std::erase(parent_->children_, this);
        parent_ = parent;
        if (parent_)
            parent_->children_.push_back(this);
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

    void GameObject::visitComponents(std::function<void(Component*)> componentCallback)
    {
        for (const auto& component : components_) {
            if (componentCallback) {
                componentCallback(component.get());
            }
        }
    }
} // namespace chai::cup