#include <Scene/LightSystem.h>
#include <Scene/Scene.h>
#include <Scene/GameObject.h>
#include <Scene/LightComponent.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Scene/TransformComponent.h>

namespace chai::cup
{
    void LightingSystem::collectLights(Scene* scene) 
    {
        lights.clear();

        // Get all GameObjects with LightComponent
		auto gameObjects = scene->getObjectsWithComponent<LightComponent>();

        for (auto& gameObject : gameObjects) {
            auto lightComp = gameObject->getComponent<LightComponent>();
            if (!lightComp || !lightComp->enabled) continue;

            if (lights.size() >= MAX_LIGHTS) break;

            Light light = {};
            light.type = static_cast<int>(lightComp->type);
            light.color = lightComp->color;
            light.intensity = lightComp->intensity;
            light.range = lightComp->range;
            light.attenuation = lightComp->attenuation;
            light.innerCone = glm::cos(glm::radians(lightComp->innerCone));
            light.outerCone = glm::cos(glm::radians(lightComp->outerCone));
            light.enabled = lightComp->enabled ? 1 : 0;

            // Get transform from GameObject
            auto transform = gameObject->getComponent<TransformComponent>();
            light.position = transform->getWorldPosition();
            light.direction = transform->forward();

            lights.push_back(light);
        }
    }
}