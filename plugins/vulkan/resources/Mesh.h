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