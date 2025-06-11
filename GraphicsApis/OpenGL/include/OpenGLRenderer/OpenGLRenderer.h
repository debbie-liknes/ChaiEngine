#pragma once
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <ChaiEngine/Renderer.h>
#include <OpenGLRenderer/GLShader.h>
#include <OpenGLRenderer/GLShaderProgram.h>
#include <ChaiEngine/Window.h>

namespace chai::brew
{
	class Renderable;
	enum ShaderStage;
	class GlPipelineState;

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
    public:
        OpenGLBackend();
        ~OpenGLBackend();

        // RendererPlugin interface
        bool initialize(void* winProcAddress = nullptr) override;
        void shutdown() override;
        void executeCommands(const std::vector<RenderCommand>& commands) override;

        // Resource factories
        std::shared_ptr<IMesh> createMesh(
            const std::vector<Vertex>& vertices,
            const std::vector<uint32_t>& indices = {}
        ) override;

        std::shared_ptr<IMaterial> createMaterial(
            const std::string& vertexShader,
            const std::string& fragmentShader
        ) override;

        // Texture management
        uint32_t createTexture(const void* data, int width, int height, int channels) override;
        //void destroyTexture(uint32_t textureId) override;

    private:
        //void executeDrawMesh(const RenderCommand& cmd);
        //void executeSetViewport(const RenderCommand& cmd);
        //void executeClear(const RenderCommand& cmd);

        //// State management
        //void bindMaterial(const IMaterial* material);
        //void bindMesh(const IMesh* mesh);

    private:
        // State tracking for batching
        const IMaterial* currentMaterial;
        GLuint currentVAO;

	};
}

CHAI_PLUGIN_CLASS(OpenGLPlugin) {
	CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::OpenGLBackend, "Renderer");
}
CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLRenderer", "1.0.0")