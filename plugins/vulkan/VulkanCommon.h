#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <ChaiMath.h>

namespace chai::gfx
{
    static constexpr uint32_t kFramesInFlight = 2;

    struct PushConstants {
        math::Mat4 model;
        int shadingMode = 0; // 0=Lit, 1=Normals, 2=UV, 3=Roughness, 4=Metallic, 5=AO
    };
}