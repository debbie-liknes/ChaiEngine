#include <Graphics/VertexAttribute.h>
#include <OpenGLRenderer/GLHelpers.h>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLTexture.h>
#include <OpenGLRenderer/Uploads.h>
#include <Resource/ResourceManager.h>

#include <chrono>

namespace chai::brew
{
    void UploadQueue::requestUpload(ResourceHandle handle,
                                    void* userData,
                                    UploadType type,
                                    void* extraData)
    {
        if (m_uploading.contains(handle.index) || m_ready.contains(handle.index)) {
            return;
        }

        m_pendingUploads.emplace(type, handle, userData, extraData);
        m_uploading.insert(handle.index);
    }

    void UploadQueue::processUploads(float timeBudgetMs)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        while (!m_pendingUploads.empty()) {
            // Check time budget
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float, std::milli>(now - startTime).count();

            if (elapsed >= timeBudgetMs) {
                break; // Save remaining uploads for next frame
            }

            // Pop next upload request
            UploadRequest request = m_pendingUploads.front();
            m_pendingUploads.pop();

            // Perform the actual GPU upload
            performUpload(request);

            // Mark as ready
            m_uploading.erase(request.handle.index);
            m_ready.insert(request.handle.index);
        }
    }

    void UploadQueue::uploadMesh(const UploadRequest& request)
    {
        checkGLError("mesh upload start");
        auto* meshData = static_cast<OpenGLMeshData*>(request.userData);

        const auto* meshResource =
            ResourceManager::instance().getResource<MeshResource>(request.handle);
        if (!meshResource) {
            std::cerr << "UploadQueue: Invalid mesh handle during upload.\n";
            return;
        }

        // Copy the layout from the resource
        meshData->layout = meshResource->vertexLayout;
        meshData->vertexCount = meshResource->vertexCount;
        meshData->indexCount = meshResource->indexData.size();

        // Get the raw vertex data from the resource
        const std::vector<uint8_t>& vertexData = meshResource->vertexData;

        if (meshData->VBO == 0) {
            glGenBuffers(1, &meshData->VBO);
            checkGLError("glGenBuffers VBO");
        }

        glBindBuffer(GL_ARRAY_BUFFER, meshData->VBO);
        checkGLError("glBindBuffer VBO");

        glBufferData(GL_ARRAY_BUFFER, vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
        checkGLError("glBufferData VBO");

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind

        if (!meshResource->indexData.empty()) {
            if (meshData->EBO == 0) {
                glGenBuffers(1, &meshData->EBO);
                checkGLError("glGenBuffers EBO");
            }

            const std::vector<uint32_t>& indices = meshResource->indexData;

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->EBO);
            checkGLError("glBindBuffer EBO");

            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indices.size() * sizeof(uint32_t),
                         indices.data(),
                         GL_STATIC_DRAW);
            checkGLError("glBufferData EBO");

            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind
        }

        meshData->isUploaded = true;
        checkGLError("mesh upload complete");

        std::cout << "Uploaded mesh: " << meshData->vertexCount << " vertices, "
                  << meshData->indexCount << " indices" << std::endl;
    }

    void UploadQueue::uploadTexture(const UploadRequest& request)
    {
        checkGLError("texture upload start");

        auto* texData = static_cast<OpenGLTextureData*>(request.userData);
        auto* texManager = static_cast<OpenGLTextureManager*>(request.extraData);

        if (!texManager) {
            std::cerr << "UploadQueue: No texture manager provided!" << std::endl;
            return;
        }

        // Upload texture via manager
        if (texManager->uploadTexture(request.handle, texData)) {
            checkGLError("texture upload complete");
            std::cout << "Texture upload completed successfully" << std::endl;
        } else {
            std::cerr << "Texture upload failed!" << std::endl;
        }
    }

    void UploadQueue::performUpload(const UploadRequest& request)
    {
        if (request.type == UploadType::MESH) {
            uploadMesh(request);
        } else if (request.type == UploadType::TEXTURE) {
            uploadTexture(request);
        }
    }

    bool UploadQueue::isQueued(ResourceHandle handle) const
    {
        if (m_uploading.contains((handle.index)))
            return true;

        std::queue<UploadRequest> temp = m_pendingUploads;
        while (!temp.empty()) {
            if (temp.front().handle.index == handle.index) {
                return true;
            }
            temp.pop();
        }
        return false;
    }

    bool UploadQueue::isReady(ResourceHandle handle) const
    {
        return m_ready.contains(handle.index);
    }
} // namespace chai::brew
