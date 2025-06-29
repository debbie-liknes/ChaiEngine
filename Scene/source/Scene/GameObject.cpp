#include <Scene/GameObject.h>
#include <Scene/TransformComponent.h>
#include <Scene/RenderableComponent.h>
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
				cmd.mesh = renderable->getMesh()->getMesh();
				cmd.material = renderable->getMesh()->getMaterial();
				cmd.transform = getComponent<TransformComponent>()->getWorldMatrix();
				//cmd.boundingBox = renderable->getBoundingBox();

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