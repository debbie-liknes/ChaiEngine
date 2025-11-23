#pragma once
#include <ChaiMath.h>

namespace chai::brew
{
    constexpr int MAX_LIGHTS = 8;

    //intended for caching by the renderer
    struct Light
    {
        Vec4 positionAndType;   // xyz = pos/dir, w = type
        Vec4 color;             // rgb = color, a = intensity
        Vec4 directionAndRange; // xyz = direction, w = range
        Vec4 spotParams;        // x = inner cone, y = outer cone
    };

    struct LightingData
    {
        Light lights[MAX_LIGHTS];
        int numLights;
    };
}