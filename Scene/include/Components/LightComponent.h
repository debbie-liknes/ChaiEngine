#pragma once
#include <glm/glm.hpp>
#include <Components/ComponentBase.h>

namespace chai::cup
{
    enum class LightType 
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    class GameObject;
    class LightComponent : public Component
    {
    public:
        LightComponent(GameObject* owner = nullptr);
        LightType type = LightType::DIRECTIONAL;
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 0.7f;

		//glm::vec3 direction = glm::vec3(-1.0f, -1.0f, -1.0f);

        // For point/spot lights
        float range = 10.0f;
        glm::vec3 attenuation = glm::vec3(1.0f, 0.09f, 0.032f); // constant, linear, quadratic

        // For spot lights
        float innerCone = 12.5f; // degrees
        float outerCone = 17.5f; // degrees

        bool enabled = true;
    };
}