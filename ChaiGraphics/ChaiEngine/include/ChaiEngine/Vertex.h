#pragma once
#include <ChaiMath.h>

namespace chai::brew
{
    struct Vertex 
    {
        Vec3 position;
        Vec3 normal;
        Vec2 texCoord;
        Vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };
}