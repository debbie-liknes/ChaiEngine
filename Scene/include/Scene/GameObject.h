#pragma once
#include <memory>
#include <ChaiEngine/RenderCommandCollector.h>
#include <Scene/ComponentBase.h>
#include <Scene/ControllerComponent.h>
#include <Core/Updatable.h>

namespace chai::cup
{
	class GameObject : public IUpdatable
	{
	public:
		GameObject();
		~GameObject() = default;

		//TODO: make sure its a Component base child
		template<typename T> T* addComponent(GameObject* owner = nullptr)
		{
			m_components.push_back(std::make_unique<T>(owner));
			return static_cast<T*>(m_components.back().get());
		}

		template<typename T> T* getComponent()
		{
			for (auto& component : m_components)
			{
				if (auto casted = dynamic_cast<T*>(component.get()))
				{
					return casted;
				}
			}
			return nullptr;
		}

        template<typename T, typename... Args>
        T* addController(Args&&... args) {
            if (!controllerComponent) {
                controllerComponent = std::make_unique<ControllerComponent>(this);
            }
            return controllerComponent->addController<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        T* getController() {
            return controllerComponent ? controllerComponent->getController<T>() : nullptr;
        }

        IController* getController(const std::string& name) {
            return controllerComponent ? controllerComponent->getController(name) : nullptr;
        }

        template<typename T>
        bool removeController() {
            return controllerComponent ? controllerComponent->removeController<T>() : false;
        }

        bool hasControllers() const {
            return controllerComponent && controllerComponent->hasControllers();
        }

        void setControllersEnabled(bool enabled) {
            if (controllerComponent) {
                controllerComponent->setAllEnabled(enabled);
            }
        }

		void collectRenderables(brew::RenderCommandCollector& collector);

		void update(double deltaTime) override;

	private:
		std::vector<std::unique_ptr<Component>> m_components;
		std::unique_ptr<ControllerComponent> controllerComponent;
	};
}