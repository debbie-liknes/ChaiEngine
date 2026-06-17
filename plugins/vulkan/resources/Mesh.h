/**
 * @file Mesh.h
 */
#pragma once
#include "Buffer.h"
#include <Rendering/IRenderer.h>
#include <ResourceFactory.h>
#include <Assets/MeshAsset.h>
#include <Assets/IMeshRegistry.h>
#include <AssetCache.h>
#include <VkBootstrap.h>
#include "../utils/VkCheck.h"
#include "../utils/VkUtils.h"
#include <vulkan/vulkan.h>
#include <array>
#include "../renderer/VulkanContext.h"

namespace chai::gfx
{
    /**
     * @brief GPU side data for a mesh
     */
    struct GpuMesh 
    {
        Buffer vertexBuffer;
        Buffer indexBuffer;
        uint32_t indexCount = 0;
    };
} // namespace chai::gfx

namespace chai
{
    /**
     * @brief Asset traits specialization for mesh
     * Must be in chai namespace, not chai::gfx
     */
    template <>
    struct AssetTraits<gfx::Mesh> {
        using Asset = gfx::MeshAsset;
        using Resource = gfx::GpuMesh;
    };
} // namespace chai

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