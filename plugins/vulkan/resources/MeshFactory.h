/**
 * @file MeshFactory.h
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
#include "Mesh.h"

namespace chai::gfx
{
    /**
     * @brief Knows how to turn a CPU mesh into a GPU resource
     */
    class MeshFactory final : public ResourceFactory<Mesh>
    {
    public:
        MeshFactory(VulkanContext& ctx, VmaAllocator allocator);

        LoadState createResource(const MeshAsset& asset, GpuMesh& out) override;
        void destroyResource(GpuMesh& res) noexcept override;
        bool discardAssetAfterUpload() const noexcept override;

        LoadState pollState(const GpuMesh&) override;

    private:

        struct PendingUpload {
            GpuMesh mesh;
            Buffer buff;
            uint64_t value;
        };
        std::vector<PendingUpload> pending_;

        VulkanContext& ctx_;
        VmaAllocator allocator_;
    };
}