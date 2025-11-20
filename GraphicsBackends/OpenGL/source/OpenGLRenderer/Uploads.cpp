#include <OpenGLRenderer/Uploads.h>
#include <chrono>
#include <OpenGLRenderer/OpenGLMesh.h>

namespace chai::brew
{
    void UploadQueue::requestUpload(Handle handle, void* userData, UploadType type)
    {
		m_pendingUploads.emplace(type, handle, userData);
    }

    void UploadQueue::processUploads(float timeBudgetMs)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        while (!m_pendingUploads.empty()) 
        {
            // Check time budget
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float, std::milli>(
                now - startTime).count();

            if (elapsed >= timeBudgetMs) 
            {
                break;  // Save remaining uploads for next frame
            }

            // Pop next upload request
            UploadRequest request = m_pendingUploads.front();
            m_pendingUploads.pop();
            m_uploading.erase(request.handle.index);

            // Perform the actual GPU upload
            performUpload(request);

            // Mark as ready
            m_ready.insert(request.handle.index);
        }
    }

    void setupVertexAttributes()
    {
        // Assuming Vertex structure: position(3f), normal(3f), texCoord(2f)
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // Texture coordinate attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);
    }

    void UploadQueue::performUpload(const UploadRequest& request)
    {
        if (request.type == UploadType::MESH) 
        {
            auto* meshData = static_cast<OpenGLMeshData*>(request.userData);

            const auto* meshPtr = chai::AssetManager::instance().get<MeshAsset>(request.handle);
            if (!meshPtr)
            {
                std::cerr << "UploadQueue: Invalid mesh handle during upload.\n";
				return;
            }

			auto& mesh = *meshPtr;

            const auto& vertices = mesh.getVertices();
            const auto& indices = mesh.getIndices();

            // Generate and upload
            glGenVertexArrays(1, &meshData->VAO);
            glGenBuffers(1, &meshData->VBO);
            glGenBuffers(1, &meshData->EBO);

            glBindVertexArray(meshData->VAO);

            glBindBuffer(GL_ARRAY_BUFFER, meshData->VBO);
            glBufferData(GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                vertices.data(),
                GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(uint32_t),
                indices.data(),
                GL_STATIC_DRAW);

            // Setup attributes
            setupVertexAttributes();

            meshData->indexCount = indices.size();
            meshData->isUploaded = true;

            glBindVertexArray(0);
        }
    }

    bool UploadQueue::isQueued(Handle handle) const
    {
        return m_uploading.contains(handle.index) ||
            !m_pendingUploads.empty();
    }

    bool UploadQueue::isReady(Handle handle) const 
    {
        return m_ready.contains(handle.index);
    }
}