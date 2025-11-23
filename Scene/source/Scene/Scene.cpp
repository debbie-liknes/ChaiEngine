#include <Scene/Scene.h>
#include <Coordinate/Spherical.h>
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>

namespace chai::cup
{
	void Scene::addGameObject(std::unique_ptr<GameObject> object)
	{
		m_objects.push_back(std::move(object));
	}

	void Scene::collectRenderables(brew::RenderCommandCollector& collector) const
	{
		//do this in a scene graph traversal?
		for(auto& object : m_objects)
		{
			object->collectRenderables(collector);
		}
	}

	void Scene::collectLights(brew::RenderCommandCollector& collector) const
	{
		brew::RenderCommand cmd;
		cmd.type = brew::RenderCommand::SET_LIGHTS;
		for (auto& object : m_objects)
		{
			auto lightComp = object->getComponent<LightComponent>();
			if (!lightComp || !lightComp->enabled) continue;

			//we should limit the number of lights to a reasonable amount
			//should we cache this? lights dont change much

			auto transform = object->getComponent<TransformComponent>();

			brew::Light light = {};
			light.positionAndType = Vec4(transform->getWorldPosition(), static_cast<int>(lightComp->type));
			light.color = Vec4(lightComp->color, lightComp->intensity);
			light.directionAndRange = Vec4(transform->forward(), lightComp->range);
			light.spotParams = Vec4(lightComp->innerCone, lightComp->outerCone, 0.0, 0.0);

			cmd.lights.push_back(light);
		}
		if (cmd.lights.empty()) return;

		collector.submit(cmd);
	}

	void Scene::update(double deltaTime)
	{
		for(auto const& object : m_objects)
		{
			object->update(deltaTime);
		}
	}
}