#pragma once
#include <glm/glm.hpp>

namespace chai::brew
{
    struct Vertex 
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}