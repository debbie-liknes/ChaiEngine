#include "MeshFactory.h"

namespace chai::gfx
{
    MeshFactory::MeshFactory(VulkanContext& ctx, VmaAllocator allocator)
        : ctx_(ctx), allocator_(allocator)
    {
    }

    LoadState MeshFactory::createResource(const MeshAsset& asset, GpuMesh& out) 
    {
        if (!asset.isValid()) {
            CHAI_LOG_ERROR("Mesh asset is invalid. Could not create Mesh Resource.");
            return LoadState::Failed;
        }

        const VkDeviceSize vbSize = asset.vertices.size() * sizeof(Vertex);
        const VkDeviceSize ibSize = asset.indices.size() * sizeof(uint32_t);

        out.indexCount = static_cast<uint32_t>(asset.indices.size());

        out.vertexBuffer =
            createBuffer(allocator_,
                         vbSize,
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         0);
        out.indexBuffer =
            createBuffer(allocator_,
                         ibSize,
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         0);

        Buffer staging = createBuffer(allocator_,
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

        uint64_t value = ctx_.uploadContext().submit([&](VkCommandBuffer cmd) {
            VkBufferCopy vb{0, 0, vbSize};
            VkBufferCopy ib{vbSize, 0, ibSize};
            vkCmdCopyBuffer(cmd, staging.handle, out.vertexBuffer.handle, 1, &vb);
            vkCmdCopyBuffer(cmd, staging.handle, out.indexBuffer.handle, 1, &ib);
        });

        pending_.emplace_back(out, staging, value);

        return LoadState::Uploading;
    }

    void MeshFactory::destroyResource(GpuMesh& res) noexcept 
    {
        // dont defer this
        destroyBufferImmediate(allocator_, res.vertexBuffer);
        destroyBufferImmediate(allocator_, res.indexBuffer);
    }

    bool MeshFactory::discardAssetAfterUpload() const noexcept 
    {
        return true;
    }

    LoadState MeshFactory::pollState(const GpuMesh& mesh)
    {
        using enum chai::LoadState;
        auto it = std::find_if(
            pending_.begin(), pending_.end(), [&](const MeshFactory::PendingUpload& upload) {
                return upload.mesh.vertexBuffer.handle == mesh.vertexBuffer.handle;
            });

        if (it != pending_.end()) {
            if (uint64_t completed = ctx_.uploadContext().completedValue();
                it->value <= completed) {
                // we completed the upload to the gpu
                destroyBufferImmediate(allocator_, it->buff);
                pending_.erase(it);
                return Ready;
            }
            return Uploading;
        }

        return Failed;
    }
}