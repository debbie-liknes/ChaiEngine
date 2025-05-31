#pragma once
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/ViewData.h>
#include <Meta/ChaiMacros.h>
#include <Plugin/PluginRegistry.h>
#include <OpenGLRenderer/GLShader.h>
#include <OpenGLRenderer/GLShaderProgram.h>

namespace chai::brew
{
	class Renderable;
	enum ShaderStage;
	class GlPipelineState;

	struct GLRenderableState {
		GLuint vao = 0;
		std::vector<GLuint> vbos;
		GLuint ebo = 0;
	};

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
	public:
		OpenGLBackend();
		~OpenGLBackend();
		void setProcAddress(void* address) override;
		void renderFrame(const RenderFrame& frame) override;
		std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) override;
	private:
		GLShader* LoadOrGetShader(const std::string& path, ShaderStage stage);
		std::unordered_map<std::string, GLShader*> m_ShaderCache;
		std::vector<std::shared_ptr<GLShaderProgram>> m_programCache;
		std::map<uint64_t, GLRenderableState> m_renderableStates;
		std::shared_ptr<GLShaderProgram> loadOrGetShaderProgram(std::vector<int> shaders, std::map<uint16_t, chai::CSharedPtr<UniformBufferBase>> ubos);
	};

	CHAI_CLASS(chai::brew::OpenGLBackend)
		CHAI_METHOD(setProcAddress)
	END_CHAI()
}