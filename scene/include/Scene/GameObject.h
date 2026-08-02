#pragma once
#include <memory>
#include <Components/ControllerComponent.h>
#include <Scene/IUpdatable.h>
#include <Components/Component.h>
#include <string>
#include <string_view>
#include <span>
#include <functional>

namespace chai::scene
{
    class Visitor;

    using GameObjectId = int32_t;

    class GameObject : public IUpdatable
    {
    public:
        GameObject();
        GameObject(const std::string& name);
        GameObject(const std::string& name, GameObjectId id);
        ~GameObject() = default;

        GameObjectId getObjectId() const { return objectId_; }
        std::string_view getObjectName() const { return name_; }

        void setParent(GameObject* parent);
        GameObject* getParent() const;
        void addChild(std::unique_ptr<GameObject> child);
        std::vector<GameObject*>& getChildren() { return children_; }
        void visitComponents(std::function<void(Component*)>);

        template <typename T>
        T* addComponent()
        {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            components_.push_back(std::make_unique<T>(this));
            return static_cast<T*>(components_.back().get());
        }

        template <typename T>
        T* getComponent()
        {
            for (auto& component : components_)
            {
                if (auto casted = dynamic_cast<T*>(component.get()))
                {
                    return casted;
                }
            }
            return nullptr;
        }

        template <typename T>
        bool removeComponent()
        {
            for (auto it = components_.begin(); it != components_.end(); ++it) 
            {
                if (dynamic_cast<T*>(it->get())) 
                {
                    components_.erase(it);
                    return true;
                }
            }
            return false;
        }

        template <typename T, typename... Args>
        T* addController(Args&&... args)
        {
            if (!controllerComponent_)
            {
                controllerComponent_ = std::make_unique<ControllerComponent>(this);
            }
            return controllerComponent_->addController<T>(std::forward<Args>(args)...);
        }

        template <typename T>
        T* getController()
        {
            return controllerComponent_ ? controllerComponent_->getController<T>() : nullptr;
        }

        IController* getController(const std::string& name)
        {
            return controllerComponent_ ? controllerComponent_->getController(name) : nullptr;
        }

        template <typename T>
        bool removeController()
        {
            return controllerComponent_ ? controllerComponent_->removeController<T>() : false;
        }

        bool hasControllers() const
        {
            return controllerComponent_ && controllerComponent_->hasControllers();
        }

        void setControllersEnabled(bool enabled) const
        {
            if (controllerComponent_)
            {
                controllerComponent_->setAllEnabled(enabled);
            }
        }

        virtual void update(const UpdateContext&) override;

        virtual void accept(Visitor* visitor);

    private:
        std::vector<std::unique_ptr<Component>> components_;
        std::unique_ptr<ControllerComponent> controllerComponent_;
        std::string name_;

        //hierarchy
        GameObject* parent_ = nullptr;
        std::vector<GameObject*> children_;

        int32_t objectId_ = -1;
    };
}