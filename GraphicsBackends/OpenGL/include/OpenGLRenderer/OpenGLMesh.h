#pragma once
#include <OpenGLRendererExport.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/IMesh.h>
#include <Types/CMap.h>
#include <glad/gl.h>
#include <memory>

#include "Graphics/ShaderAsset.h"

namespace chai::brew
{
    struct OPENGLRENDERER_EXPORT OpenGLMeshData
    {
        GLuint VBO = 0;
        GLuint EBO = 0;

        CMap<GLuint, GLuint> vaosPerShader;

        VertexLayout layout; // Copy for convenience
        uint32_t vertexCount;
        uint32_t indexCount;
        bool isUploaded = false;
    };

    class OPENGLRENDERER_EXPORT OpenGLMeshManager
    {
    public:
        OpenGLMeshManager() = default;
        virtual ~OpenGLMeshManager();

        OpenGLMeshManager(const OpenGLMeshManager&) = delete;
        OpenGLMeshManager& operator=(const OpenGLMeshManager&) = delete;

        OpenGLMeshManager(OpenGLMeshManager&&) = default;
        OpenGLMeshManager& operator=(OpenGLMeshManager&&) = default;

        OpenGLMeshData* getOrCreateMeshData(Handle meshHandle)
        {
            auto it = m_meshCache.find(meshHandle.index);
            if (it == m_meshCache.end())
            {
                auto glMeshData = std::make_unique<OpenGLMeshData>();
                auto* ptr = glMeshData.get();
                m_meshCache[meshHandle.index] = std::move(glMeshData);
                return m_meshCache[meshHandle.index].get();
            }
            return it->second.get();
        }

        GLuint getOrCreateVAO(OpenGLMeshData* meshData,
                              GLuint shaderProgram,
                              const ShaderAsset* shaderAsset);

    private:
        CMap<size_t, std::unique_ptr<OpenGLMeshData>> m_meshCache;
    };
}