#pragma once
#include <queue>
#include <unordered_set>
#include <Asset/AssetHandle.h>

namespace chai::brew
{
    enum class UploadType
    {
        MESH,
        TEXTURE,
        SHADER
    };

    struct UploadRequest
    {
        UploadType type;
        ResourceHandle handle;
        void* userData;
    };

    class UploadQueue
    {
    public:
        void requestUpload(ResourceHandle handle, void* userData, UploadType type = UploadType::MESH);
        void processUploads(float timeBudgetMs);

        bool isQueued(ResourceHandle handle) const;
        bool isReady(ResourceHandle handle) const;

    private:
        void performUpload(const UploadRequest& request);

        std::queue<UploadRequest> m_pendingUploads;
        std::unordered_set<uint32_t> m_uploading; // Currently processing
        std::unordered_set<uint32_t> m_ready; // Completed uploads
    };
}