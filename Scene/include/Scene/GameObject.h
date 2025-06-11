#pragma once
#include <Renderables/Renderable.h>
#include <Renderables/Cube.h>
#include <memory>
#include <Scene/ComponentBase.h>

namespace chai::cup
{
	class GameObject
	{
	public:
		GameObject() = default;
		~GameObject() = default;

		template<typename T> T* addComponent()
		{
			components.push_back(std::make_unique<T>());
			return static_cast<T*>(components.back().get());
		}

	private:
		std::vector<std::unique_ptr<Component>> components;
	};
}