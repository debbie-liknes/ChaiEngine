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

	struct GLRenderableState 
	{
		GLuint vao = 0;
		std::vector<GLuint> vbos;
		GLuint ebo = 0;
	};

	class OPENGLRENDERER_EXPORT OpenGLCommandList : public RenderCommandList 
	{
		std::vector<std::function<void()>> commands;

	public:
		void setViewport(const Viewport& vp) override;
		//this should really be by value
		void drawMesh(const IMesh& mesh, const IMaterial& material) override;
		void execute();
	};

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
	public:
		OpenGLBackend();
		~OpenGLBackend();
		virtual std::unique_ptr<RenderCommandList> createCommandList() override;
		virtual void executeCommandList(RenderCommandList& cmdList) override;
		//this is a weird parameter, rethink this
		virtual void present(Window& window) override;

		//void renderFrame(const RenderFrame& frame) override;
		//std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) override;
	private:
		//GLShader* LoadOrGetShader(const std::string& path, ShaderStage stage);
		//std::unordered_map<std::string, GLShader*> m_ShaderCache;
		//std::vector<std::shared_ptr<GLShaderProgram>> m_programCache;
		//std::map<uint64_t, GLRenderableState> m_renderableStates;
		//std::shared_ptr<GLShaderProgram> loadOrGetShaderProgram(std::vector<int> shaders, std::map<uint16_t, chai::CSharedPtr<UniformBufferBase>> ubos);
	};
}

CHAI_PLUGIN_CLASS(OpenGLPlugin) {
	CHAI_SERVICE(chai::brew::OpenGLBackend, "renderer");
}
CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLRenderer", "1.0.0")