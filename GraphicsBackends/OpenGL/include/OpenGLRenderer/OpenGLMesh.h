#pragma once
#include <OpenGLRendererExport.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/Vertex.h>
#include <Types/CMap.h>
#include <glad/gl.h>
#include <vector>
#include <memory>

namespace chai::brew
{
    //struct OpenGLMeshData 
    //{
    //    GLuint VAO = 0;
    //    GLuint VBO = 0;
    //    GLuint EBO = 0;
    //    size_t indexCount = 0;
    //    bool isUploaded = false;

    //    ~OpenGLMeshData() 
    //    {
    //        if (VAO) glDeleteVertexArrays(1, &VAO);
    //        if (VBO) glDeleteBuffers(1, &VBO);
    //        if (EBO) glDeleteBuffers(1, &EBO);
    //    }
    //};

    struct OPENGLRENDERER_EXPORT OpenGLMeshData 
    {
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
        GLuint instanceVBO = 0;     // For instanced rendering
        uint32_t indexCount = 0;
        uint32_t maxInstances = 0;
        bool isUploaded = false;
        bool supportsInstancing = false;
    };

    class OPENGLRENDERER_EXPORT OpenGLMeshManager
    {
    public:
        OpenGLMeshManager() = default;
        virtual ~OpenGLMeshManager() = default;

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
                return ptr;
            }
            return it->second.get();
        }

    private:
        CMap<size_t, std::unique_ptr<OpenGLMeshData>> m_meshCache;
    };
}