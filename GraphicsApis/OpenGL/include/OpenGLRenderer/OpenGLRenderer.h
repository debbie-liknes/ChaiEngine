#pragma once
#include <OpenGLRendererExport.h>
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

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
	public:
		OpenGLBackend();
		~OpenGLBackend() {}
		void setProcAddress(void* address) override;
		void renderFrame(const RenderFrame& frame) override;
		//void renderFrame(chai::Window* window, chai::CVector<chai::CSharedPtr<Renderable>> ros, ViewData data) override;
		std::shared_ptr<Shader> LoadOrGetShader(const std::string& path, ShaderStage stage) override;
		std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) override;
	private:
		std::unordered_map<std::string, std::shared_ptr<GLShader>> m_ShaderCache;
		std::vector<std::shared_ptr<GLShaderProgram>> m_programCache;
		std::shared_ptr<GLShaderProgram> loadOrGetShaderProgram(std::vector<int> shaders, std::map<uint16_t, chai::CSharedPtr<UniformBufferBase>> ubos);
	};

	CHAI_CLASS(chai::brew::OpenGLBackend)
		CHAI_METHOD(setProcAddress)
	END_CHAI()
}

void registerServices()
{
	chai::kettle::PluginRegistry::Instance().Register("Renderer", "OpenGL", [] {
		return static_cast<void*>(new chai::brew::OpenGLBackend());
		});
}

CHAI_PLUGIN(OpenGL, "1.0.0", "chai::brew", "Renderer", registerServices)