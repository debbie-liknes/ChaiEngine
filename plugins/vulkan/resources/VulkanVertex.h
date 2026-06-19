#pragma once
#include <vulkan/vulkan.h>
#include <array>
#include <Geometry/Vertex.h>

namespace chai::gfx
{
    inline VkVertexInputBindingDescription vertexBinding()
    {
        return {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    }

    inline std::array<VkVertexInputAttributeDescription, 4> vertexAttributes()
    {
        return {{
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
            {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)},
            {3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, tangent)},
        }};
    }
}