#pragma once
#include "../VulkanCommon.h"
#include <Common/GraphicsEnums.h>
#include <string>

namespace chai::gfx
{
    struct PipelineKey
    {
        std::string vertShader;
        std::string fragShader;
        AlphaMode alpha = AlphaMode::Opaque;
        VkPolygonMode polygon = VK_POLYGON_MODE_FILL;

        bool operator==(const PipelineKey&) const = default;
    };

    struct PipelineKeyHash {
        size_t operator()(const PipelineKey& k) const
        {
            size_t h = std::hash<std::string>{}(k.vertShader);
            h ^= std::hash<std::string>{}(k.fragShader) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(static_cast<int>(k.alpha)) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(static_cast<int>(k.polygon)) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };
} // namespace chai::gfx