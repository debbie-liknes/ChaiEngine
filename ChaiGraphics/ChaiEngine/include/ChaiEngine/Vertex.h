#pragma once
#include <glm/glm.hpp>
#include <ChaiMath.h>

namespace chai::brew
{
    struct Vertex 
    {
        glm::vec3 position;
        glm::vec3 normal;
        Vec2 texCoord;
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}