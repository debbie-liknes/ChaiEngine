#pragma once
#include <memory>
#include <ChaiEngine/RenderCommandCollector.h>
#include <Scene/ComponentBase.h>

namespace chai::cup
{
	class GameObject
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

		void collectRenderables(brew::RenderCommandCollector& collector);

	private:
		std::vector<std::unique_ptr<Component>> m_components;
	};
}