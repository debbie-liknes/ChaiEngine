#pragma once
#include <ChaiMath.h>

namespace chai::brew
{
    struct Light 
    {
        int type; // 0=directional, 1=point, 2=spot
        Vec3 position;
        Vec3 direction;
        Vec3 color;
        float intensity;
        float range;
        Vec3 attenuation;
        float innerCone;
        float outerCone;
        int enabled;
        float _padding[2]; // For 16-byte alignment in UBO
    };
}