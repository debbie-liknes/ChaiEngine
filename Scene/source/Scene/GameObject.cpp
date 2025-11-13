#include <Scene/GameObject.h>
#include <Components/TransformComponent.h>
#include <Components/RenderableComponent.h>
#include <Core/Updatable.h>

namespace chai::cup
{
	GameObject::GameObject()
	{
		addComponent<TransformComponent>(this);
	}

	void GameObject::collectRenderables(brew::RenderCommandCollector& collector)
	{
		for (const auto& component : m_components)
		{
			if (auto renderable = dynamic_cast<RenderableComponent*>(component.get()))
			{
				brew::RenderCommand cmd;
				cmd.type = brew::RenderCommand::DRAW_MESH;
				cmd.mesh = renderable->getMesh();
				cmd.material = renderable->getMaterial();
				cmd.transform = getComponent<TransformComponent>()->getWorldMatrix();

				collector.submit(cmd);
			}
		}
	}

	void GameObject::update(double deltaTime)
	{
		for (const auto& component : m_components)
		{
			if (auto updatable = dynamic_cast<IUpdatable*>(component.get()))
			{
				updatable->update(deltaTime);
			}
		}

		if(controllerComponent)
		{
			controllerComponent->update(deltaTime);
		}
	}
}