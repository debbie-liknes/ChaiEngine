#pragma once
#include "../VulkanCommon.h"

#include <Common/GraphicsEnums.h>
#include <string>

namespace chai::gfx
{
    struct VertexInputDesc {
        std::vector<VkVertexInputAttributeDescription> attributes;
        VkVertexInputBindingDescription binding{};

        bool operator==(const VertexInputDesc&) const = default;
    };

    struct RasterState {
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

        bool operator==(const RasterState&) const = default;
    };

    struct DepthState {
        bool test = false;
        bool write = false;
        bool bias = false;
        VkCompareOp compareOp = VK_COMPARE_OP_LESS;
        VkFormat format = VK_FORMAT_UNDEFINED;

        bool operator==(const DepthState&) const = default;
    };

    struct ColorState {
        VkFormat format = VK_FORMAT_UNDEFINED;
        bool blending = false;

        bool operator==(const ColorState&) const = default;
    };

    struct PipelineDesc {
        std::string vertShader;
        std::string fragShader;

        VertexInputDesc vertexInput;
        RasterState raster;
        DepthState depth;
        ColorState color;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        bool operator==(const PipelineDesc&) const = default;
    };

    struct PipelineKey {
        VkPipelineLayout layout;
        PipelineDesc desc;

        bool operator==(const PipelineKey&) const = default;
    };

    //TODO: generational handle
    struct PipelineHandle {
        uint32_t index = UINT32_MAX;

        bool valid() const { return index != UINT32_MAX; }
    };

} // namespace chai::gfx