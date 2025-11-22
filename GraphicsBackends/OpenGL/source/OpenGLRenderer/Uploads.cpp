#include <OpenGLRenderer/Uploads.h>
#include <chrono>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <Resource/ResourceManager.h>
#include <Graphics/VertexAttribute.h>

#include "Graphics/ShaderAsset.h"
#include "OpenGLRenderer/GLHelpers.h"

namespace chai::brew {
    void UploadQueue::requestUpload(ResourceHandle handle, void *userData, UploadType type) {
        m_pendingUploads.emplace(type, handle, userData);
    }

    void UploadQueue::processUploads(float timeBudgetMs) {
        auto startTime = std::chrono::high_resolution_clock::now();

        while (!m_pendingUploads.empty()) {
            // Check time budget
            auto now = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float, std::milli>(
                now - startTime).count();

            if (elapsed >= timeBudgetMs) {
                break; // Save remaining uploads for next frame
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

    void UploadQueue::performUpload(const UploadRequest &request) {
        if (request.type == UploadType::MESH) {
            checkGLError("mesh upload");
            auto *meshData = static_cast<OpenGLMeshData *>(request.userData);

            const auto *meshResource = ResourceManager::instance().getResource<MeshResource>(request.handle);
            if (!meshResource) {
                std::cerr << "UploadQueue: Invalid mesh handle during upload.\n";
                return;
            }

            // Generate VAO/VBO if not already done
            if (meshData->VAO == 0) {
                glGenVertexArrays(1, &meshData->VAO);
                checkGLError("mesh upload");
                glGenBuffers(1, &meshData->VBO);
                checkGLError("mesh upload");

                // Check if we have indices
                if (!meshResource->indexData.empty()) {
                    glGenBuffers(1, &meshData->EBO);
                    checkGLError("mesh upload");
                }
            }

            // Copy the layout from the resource (it already knows its structure!)
            meshData->layout = meshResource->vertexLayout;

            // Get the raw vertex data from the resource
            const std::vector<uint8_t> &vertexData = meshResource->vertexData;

            //debugging
            std::vector<float> floats(vertexData.size() / 4);
            std::memcpy(floats.data(), vertexData.data(), vertexData.size());
            //end debugging

            meshData->vertexCount = meshResource->vertexCount;
            meshData->indexCount = meshResource->indexData.size();

            // Upload vertex data to GPU
            glBindVertexArray(meshData->VAO);
            checkGLError("mesh upload");

            meshData->vertexCount = 3;

            glBindBuffer(GL_ARRAY_BUFFER, meshData->VBO);
            glBufferData(GL_ARRAY_BUFFER, vertexData.size(), vertexData.data(), GL_STATIC_DRAW);

            // Upload index data if present
            if (meshData->EBO != 0 && meshData->indexCount > 0) {
                const std::vector<uint32_t> &indices = meshResource->indexData;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->EBO);
                checkGLError("mesh upload");
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             indices.size() * sizeof(uint32_t),
                             indices.data(),
                             GL_STATIC_DRAW);
                checkGLError("mesh upload");
            }

            // This is temporary, makes things inflexible tho
            for (size_t i = 0; i < meshData->layout.getAttributes().size(); ++i) {
                const auto& attr = meshData->layout.getAttributes()[i];

                glEnableVertexAttribArray(i);
                checkGLError("mesh upload");
                glVertexAttribPointer(
                    i,  // Use sequential locations for now
                    attr.getComponentCount(),
                    toGLType(attr.type),
                    attr.normalized ? GL_TRUE : GL_FALSE,
                    meshData->layout.getStride(),
                    (void*)(uintptr_t)attr.offset
                );
                checkGLError("mesh upload");
            }

            glBindVertexArray(0);

            meshData->isUploaded = true;
            checkGLError("mesh upload");
        }
    }

    bool UploadQueue::isQueued(ResourceHandle handle) const {
        return m_uploading.contains(handle.index) ||
               !m_pendingUploads.empty();
    }

    bool UploadQueue::isReady(ResourceHandle handle) const {
        return m_ready.contains(handle.index);
    }
}
