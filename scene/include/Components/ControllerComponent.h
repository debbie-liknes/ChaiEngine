#pragma once
#include <memory>
#include <vector>
#include <typeindex>
#include <string>
#include <Controllers/Controller.h>
#include <Core/Updatable.h>
#include <Components/Component.h>
#include <unordered_map>

namespace chai::scene
{
    class GameObject;

    class ControllerComponent : public Component, public IUpdatable
    {
    public:
        ControllerComponent() = default;
        explicit ControllerComponent(chai::scene::GameObject* owner);

        // Add a controller
        template <typename T, typename... Args>
        T* addController(Args&&... args)
        {
            static_assert(std::is_base_of_v<IController, T>, "T must inherit from IController");

            auto controller = std::make_unique<T>(m_owner, std::forward<Args>(args)...);
            T* ptr = controller.get();

            // Store by type
            controllersByType_[std::type_index(typeid(T))] = ptr;

            // Store by name if it has one
            if (std::string name = controller->getControllerType(); !name.empty())
            {
                controllersByName_[name] = ptr;
            }

            controllers_.push_back(std::move(controller));
            return ptr;
        }

        // Get controller by type
        template <typename T>
        T* getController()
        {
            if (auto it = controllersByType_.find(std::type_index(typeid(T))); it != controllersByType_.end())
            {
                return static_cast<T*>(it->second);
            }
            return nullptr;
        }

        // Get controller by name
        IController* getController(const std::string& name)
        {
            auto it = controllersByName_.find(name);
            return (it != controllersByName_.end()) ? it->second : nullptr;
        }

        // Remove controller
        template <typename T>
        bool removeController()
        {
            if (auto it = controllersByType_.find(std::type_index(typeid(T))); it != controllersByType_.end())
            {
                IController const* controller = it->second;

                // Remove from all maps
                controllersByType_.erase(it);
                for (auto mapIt = controllersByName_.begin(); mapIt != controllersByName_.end(); ++mapIt)
                {
                    if (mapIt->second == controller)
                    {
                        controllersByName_.erase(mapIt);
                        break;
                    }
                }

                // Remove from vector
                controllers_.erase(
                    std::remove_if(controllers_.begin(), controllers_.end(),
                                   [controller](const auto& ptr) { return ptr.get() == controller; }),
                    controllers_.end()
                );

                return true;
            }
            return false;
        }

        // Update all controllers
        void update(const UpdateContext& ctx) override
        {
            for (auto const& controller : controllers_)
            {
                if (controller->isEnabled())
                {
                    controller->update(ctx);
                }
            }
        }

        // Enable/disable all controllers
        void setAllEnabled(bool enabled) const
        {
            for (auto const& controller : controllers_)
            {
                controller->setEnabled(enabled);
            }
        }

        // Get all controllers
        const std::vector<std::unique_ptr<IController>>& getControllers() const
        {
            return controllers_;
        }

        size_t getControllerCount() const { return controllers_.size(); }
        bool hasControllers() const { return !controllers_.empty(); }

    private:
        std::vector<std::unique_ptr<IController>> controllers_;
        std::unordered_map<std::type_index, IController*> controllersByType_;
        std::unordered_map<std::string, IController*> controllersByName_;
        chai::scene::GameObject* m_owner;
    };

    CHAI_REFLECT(ControllerComponent, "ControllerComponent") {}
}