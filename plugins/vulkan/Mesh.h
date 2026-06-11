#pragma once
#include "Buffer.h"
#include <Renderer.h>
#include <ResourceFactory.h>
#include <MeshAsset.h>
#include <AssetCache.h>
#include <VkBootstrap.h>
#include "core/VkCheck.h"

namespace chai::gfx
{
    struct GpuMesh {
        Buffer vertexBuffer;
        Buffer indexBuffer;
        uint32_t indexCount = 0;
    };
} // namespace chai::gfx

namespace chai
{
    template <>
    struct AssetTraits<gfx::Mesh> {
        using Asset = gfx::MeshAsset;
        using Resource = gfx::GpuMesh;
    };
} // namespace chai

namespace chai::gfx
{
    class MeshFactory final : public ResourceFactory<Mesh>
    {
    public:
        MeshFactory(VulkanContext& ctx, VmaAllocator allocator) : ctx_(ctx), allocator_(allocator)
        {
        }

        bool loadAsset(AssetId id, MeshAsset& out) override
        {
            // resolve id -> path, parse glTF into `out` (step 6 work)
            return /* loaded */ out.isValid();
        }

        void immediateSubmit(VulkanContext& ctx,
                             std::function<void(VkCommandBuffer)>&& fn)
        {
            const VkFence fence = ctx.immediateFence(); // by-value accessor is fine
            const VkCommandBuffer cmd = ctx.immediateCmd();

            // Fence starts unsignaled; reset is belt-and-suspenders for the 2nd+ call,
            // since the previous call left it signaled after its wait.
            VK_CHECK(vkResetFences(ctx.device(), 1, &fence));
            VK_CHECK(vkResetCommandBuffer(cmd, 0));

            VkCommandBufferBeginInfo begin{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
            begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK(vkBeginCommandBuffer(cmd, &begin));

            fn(cmd); // caller's commands: the buffer copy, later a blit, etc.

            VK_CHECK(vkEndCommandBuffer(cmd));

            VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
            submit.commandBufferCount = 1;
            submit.pCommandBuffers = &cmd;

            // Submit to the graphics queue for now. If a dedicated transfer queue ever
            // appears on the context, this is the one line that changes.
            VK_CHECK(vkQueueSubmit(ctx.graphicsQueue(), 1, &submit, fence));
            VK_CHECK(vkWaitForFences(ctx.device(), 1, &fence, VK_TRUE, UINT64_MAX)); // <-- the block
        }

        LoadState createResource(const MeshAsset& asset, GpuMesh& out) override
        {
            if (!asset.isValid()) {
                CHAI_LOG_ERROR("Mesh asset is invalid. Could not create Mesh Resource.");
                return {};
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
                return {};
            }

            auto* base = static_cast<std::byte*>(staging.info.pMappedData);
            std::memcpy(base, asset.vertices.data(), vbSize);
            std::memcpy(base + vbSize, asset.indices.data(), ibSize);
            // No-op on HOST_COHERENT memory; correct if AUTO picked a non-coherent type.
            vmaFlushAllocation(allocator_, staging.allocation, 0, VK_WHOLE_SIZE);

            immediateSubmit(ctx_, [&](VkCommandBuffer cmd) {
                VkBufferCopy vb{0, 0, vbSize}; // srcOffset, dstOffset, size
                VkBufferCopy ib{vbSize, 0, ibSize};
                vkCmdCopyBuffer(cmd, staging.handle, out.vertexBuffer.handle, 1, &vb);
                vkCmdCopyBuffer(cmd, staging.handle, out.indexBuffer.handle, 1, &ib);
            });

            // immediateSubmit fence-waited, so the copy is done — staging is safe NOW.
            destroyBufferImmediate(allocator_, staging);
            return LoadState::Ready; // synchronous: immediateSubmit blocked til done
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
            cache_->release(h); // forward
        }

        Handle<Mesh> load(AssetId) override
        { return {};
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