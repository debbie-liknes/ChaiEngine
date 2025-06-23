#include <Scene/GameObject.h>
#include <Scene/TransformComponent.h>
#include <Scene/RenderableComponent.h>

namespace chai::cup
{
	GameObject::GameObject()
	{
		addComponent<TransformComponent>();
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
				cmd.material = renderable->getMaterial();
				cmd.transform = getComponent<TransformComponent>()->getWorldMatrix();
				//cmd.boundingBox = renderable->getBoundingBox();

				collector.submit(cmd);
			}
		}
	}
}