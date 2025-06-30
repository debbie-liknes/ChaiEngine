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

			brew::Light light = {};
			light.type = static_cast<int>(lightComp->type);
			light.color = lightComp->color;
			light.intensity = lightComp->intensity;
			light.range = lightComp->range;
			light.attenuation = lightComp->attenuation;
			light.innerCone = glm::cos(glm::radians(lightComp->innerCone));
			light.outerCone = glm::cos(glm::radians(lightComp->outerCone));
			light.enabled = lightComp->enabled ? 1 : 0;
			//light.direction = lightComp->direction;

			// Get transform from GameObject
			auto transform = object->getComponent<TransformComponent>();
			light.position = transform->getWorldPosition();
			light.direction = transform->forward();

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