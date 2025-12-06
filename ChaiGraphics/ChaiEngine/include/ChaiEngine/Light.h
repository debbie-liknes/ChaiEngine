#pragma once
#include <ChaiMath.h>

namespace chai::brew
{
    constexpr int MAX_LIGHTS = 8;

    // Small object packed for the shaders
    struct Light
    {
        Vec4 positionAndType;   // xyz = pos/dir, w = type
        Vec4 color;             // rgb = color, a = intensity
        Vec4 directionAndRange; // xyz = direction, w = range
        Vec4 spotParams;        // x = inner cone, y = outer cone
    };

    struct LightInfo
    {
        Light light;
        bool castsShadow;
    };

    struct LightingData
    {
        Light lights[MAX_LIGHTS];
        int numLights;
        Vec3 padding;
    };
}