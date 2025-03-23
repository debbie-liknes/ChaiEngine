#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/ViewData.h>

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
		void renderFrame(chai::Window* window, chai::CVector<chai::CSharedPtr<Renderable>> ros, ViewData data) override;
		std::shared_ptr<Shader> LoadOrGetShader(const std::string& path, ShaderStage stage) override;
	private:
		int createShaderProgram(chai::CVector<int> shaders);
		int createShader(const char* source, ShaderStage stage);
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderCache;
	};
}

extern "C" OPENGLRENDERER_EXPORT chai::brew::OpenGLBackend* RegisterBackend() {
	return new chai::brew::OpenGLBackend();
}