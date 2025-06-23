#pragma once
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <memory>
#include <unordered_map>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::brew
{
	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
    public:
        OpenGLBackend();
        ~OpenGLBackend();

        OpenGLBackend(const OpenGLBackend&) = delete;
        OpenGLBackend& operator=(const OpenGLBackend&) = delete;

        OpenGLBackend(OpenGLBackend&&) = default;
        OpenGLBackend& operator=(OpenGLBackend&&) = default;

        // RendererPlugin interface
        bool initialize(void* winProcAddress = nullptr) override;
        void shutdown() override;
        void executeCommands(const std::vector<RenderCommand>& commands) override;

    private:
        OpenGLMeshData* getOrCreateMeshData(IMesh* mesh);
        OpenGLMaterialData* getOrCreateMaterialData(IMaterial* material);
		void drawMesh(const RenderCommand& cmd);
        void uploadMeshToGPU(IMesh* mesh, OpenGLMeshData* glMeshData);
        void setupVertexAttributes();
        void compileMaterial(IMaterial* material, OpenGLMaterialData* glMaterialData);
        void bindShaderProgram(GLuint program);
        void bindVertexArray(GLuint vao);
        GLuint createDefaultShaderProgram();
        GLuint compileShader(const char* source, GLenum type);
        void clear(float r, float g, float b, float a);

        std::unordered_map<IMesh*, std::unique_ptr<OpenGLMeshData>> m_meshCache;
        std::unordered_map<IMaterial*, std::unique_ptr<OpenGLMaterialData>> m_materialCache;

        // Current render state
        GLuint currentShaderProgram = 0;
        GLuint currentVAO = 0;

        // Default shader for basic rendering
        GLuint defaultShaderProgram = 0;
	};
}

CHAI_PLUGIN_CLASS(OpenGLPlugin) {
	CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::OpenGLBackend, "Renderer");
}
CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLRenderer", "1.0.0")