#pragma once
#include <memory>
#include <vector>
#include <typeindex>
#include <string>
#include <Scene/Controller.h>
#include <unordered_map>
#include <Scene/ComponentBase.h>
#include <Core/Updatable.h>

namespace chai::cup
{
	class GameObject;
    class ControllerComponent : public Component, public IUpdatable
    {
    public:
        ControllerComponent(chai::cup::GameObject* owner);

        // Add a controller
        template<typename T, typename... Args>
        T* addController(Args&&... args) 
        {
            static_assert(std::is_base_of_v<IController, T>, "T must inherit from IController");

            auto controller = std::make_unique<T>(m_owner, std::forward<Args>(args)...);
            T* ptr = controller.get();

            // Store by type
            controllersByType[std::type_index(typeid(T))] = ptr;

            // Store by name if it has one
            std::string name = controller->getControllerType();
            if (!name.empty()) {
                controllersByName[name] = ptr;
            }

            controllers.push_back(std::move(controller));
            return ptr;
        }

        // Get controller by type
        template<typename T>
        T* getController() 
        {
            auto it = controllersByType.find(std::type_index(typeid(T)));
            if (it != controllersByType.end()) {
                return static_cast<T*>(it->second);
            }
            return nullptr;
        }

        // Get controller by name
        IController* getController(const std::string& name) 
        {
            auto it = controllersByName.find(name);
            return (it != controllersByName.end()) ? it->second : nullptr;
        }

        // Remove controller
        template<typename T>
        bool removeController() 
        {
            auto it = controllersByType.find(std::type_index(typeid(T)));
            if (it != controllersByType.end()) {
                IController* controller = it->second;

                // Remove from all maps
                controllersByType.erase(it);
                for (auto mapIt = controllersByName.begin(); mapIt != controllersByName.end(); ++mapIt) {
                    if (mapIt->second == controller) {
                        controllersByName.erase(mapIt);
                        break;
                    }
                }

                // Remove from vector
                controllers.erase(
                    std::remove_if(controllers.begin(), controllers.end(),
                        [controller](const auto& ptr) { return ptr.get() == controller; }),
                    controllers.end()
                );

                return true;
            }
            return false;
        }

        // Update all controllers
        void update(double deltaTime) override
        {
            for (auto& controller : controllers) {
                if (controller->isEnabled()) {
                    controller->update(deltaTime);
                }
            }
        }

        // Enable/disable all controllers
        void setAllEnabled(bool enabled) 
        {
            for (auto& controller : controllers) 
            {
                controller->setEnabled(enabled);
            }
        }

        // Get all controllers
        const std::vector<std::unique_ptr<IController>>& getControllers() const 
        {
            return controllers;
        }

        size_t getControllerCount() const { return controllers.size(); }
        bool hasControllers() const { return !controllers.empty(); }

    private:
        std::vector<std::unique_ptr<IController>> controllers;
        std::unordered_map<std::type_index, IController*> controllersByType;
        std::unordered_map<std::string, IController*> controllersByName;
        chai::cup::GameObject* m_owner;

    };
}