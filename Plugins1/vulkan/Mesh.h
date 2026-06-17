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
#include "core/VkCheck.h"
#include "VkUtils.h"

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
    /**
     * @brief Knows how to turn a CPU mesh into a GPU resource
     */
    class MeshFactory final : public ResourceFactory<Mesh>
    {
    public:
        MeshFactory(VulkanContext& ctx, VmaAllocator allocator) : ctx_(ctx), allocator_(allocator)
        {
        }

        LoadState createResource(const MeshAsset& asset, GpuMesh& out) override
        {
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("Mesh asset is invalid. Could not create Mesh Resource.");
                return LoadState::Failed;
            }

            const VkDeviceSize vbSize = asset.vertices.size() * sizeof(Vertex);
            const VkDeviceSize ibSize = asset.indices.size() * sizeof(uint32_t);

            out.indexCount = static_cast<uint32_t>(asset.indices.size());

            out.vertexBuffer = createBuffer(allocator_,
                                                 vbSize,
                                                 VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                 0);
            out.indexBuffer = createBuffer(allocator_,
                                                ibSize,
                                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                0);

            Buffer staging =
                createBuffer(allocator_,
                                 vbSize + ibSize,
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                     VMA_ALLOCATION_CREATE_MAPPED_BIT);

            if (!out.vertexBuffer.valid() || !out.indexBuffer.valid() || !staging.valid()) {
                destroyBufferImmediate(allocator_, out.vertexBuffer);
                destroyBufferImmediate(allocator_, out.indexBuffer);
                destroyBufferImmediate(allocator_, staging);
                return LoadState::Failed;
            }

            auto* base = static_cast<std::byte*>(staging.info.pMappedData);
            std::memcpy(base, asset.vertices.data(), vbSize);
            std::memcpy(base + vbSize, asset.indices.data(), ibSize);

            vmaFlushAllocation(allocator_, staging.allocation, 0, VK_WHOLE_SIZE);

            //immediately submitting everything forever is not a good idea
            immediateSubmit(ctx_, [&](VkCommandBuffer cmd) {
                VkBufferCopy vb{0, 0, vbSize};
                VkBufferCopy ib{vbSize, 0, ibSize};
                vkCmdCopyBuffer(cmd, staging.handle, out.vertexBuffer.handle, 1, &vb);
                vkCmdCopyBuffer(cmd, staging.handle, out.indexBuffer.handle, 1, &ib);
            });

            // we did the immediate submissions,s o we're good
            destroyBufferImmediate(allocator_, staging);
            return LoadState::Ready; //already done
        }

        void destroyResource(GpuMesh& res) noexcept override
        {
            // dont defer this
            destroyBufferImmediate(allocator_, res.vertexBuffer);
            destroyBufferImmediate(allocator_, res.indexBuffer);
        }

        bool discardAssetAfterUpload() const noexcept override { return true; }

    private:
        VulkanContext& ctx_;
        VmaAllocator allocator_;
    };

    /**
     * @brief The registry allows the core side to add meshes to the cache. The Mesh cache belogns to
     * the renderer
     */
    class MeshRegistry final : public IMeshRegistry
    {
    public:
        MeshRegistry(MeshFactory& factory, DeferredDeleteQueue& graveyard)
        {
            cache_ = std::make_shared<AssetCache<Mesh>>(factory, graveyard);
        }

        Handle<Mesh> ingest(AssetId id, MeshAsset asset) override
        {
            return cache_->ingest(id, std::move(asset)); // forward to the real cache
        }
        void release(Handle<Mesh> h) override
        {
            cache_->release(h);
        }

        Handle<Mesh> load(AssetId) override
        { 
            return {};
        }

        std::shared_ptr<AssetCache<Mesh>> cache() { return cache_; } // plugin-internal access for the renderer

    private:
        std::shared_ptr<AssetCache<Mesh>> cache_; // the template lives HERE, plugin-side, never escapes
    };

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