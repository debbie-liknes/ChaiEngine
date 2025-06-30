#pragma once
#include <glm/glm.hpp>

namespace chai::brew
{
    struct Light 
    {
        int type; // 0=directional, 1=point, 2=spot
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float intensity;
        float range;
        glm::vec3 attenuation;
        float innerCone;
        float outerCone;
        int enabled;
        float _padding[2]; // For 16-byte alignment in UBO
    };
}