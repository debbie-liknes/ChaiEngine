#pragma once
#include <glm/glm.hpp>

namespace chai::cup
{
    enum class LightType {
        Directional,
        Point,
        Spot
    };

    struct Light {
        LightType type = LightType::Point;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float range = 10.0f; // for point/spot
        float innerConeAngle = 0.0f; // radians, for spotlights
        float outerConeAngle = 0.5f; // radians
        bool castsShadows = false;
    };
}