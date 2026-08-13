#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include <ChaiMath.h>
#include <filesystem>
#include <Runtime/SystemPaths.h>
#include "ShaderPaths.h"

namespace chai::gfx
{
    static constexpr uint32_t kFramesInFlight = 2;

    inline std::filesystem::path shaderDir()
    {
        static const std::filesystem::path sourceDir{CHAI_SHADER_SOURCE_DIR};
        if (std::filesystem::exists(sourceDir))
            return sourceDir;

        return executableDir() / "shaders"; // the post-build-copied location
    }

    struct PushConstants {
        math::Mat4 model;
        int shadingMode = 0; // 0=Lit, 1=Normals, 2=UV, 3=Roughness, 4=Metallic, 5=AO
    };
}