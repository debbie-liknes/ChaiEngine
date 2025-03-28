#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/ViewData.h>
#include <Meta/ChaiMacros.h>
#include <Core/PluginRegistry.h>

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

	CHAI_CLASS(chai::brew::OpenGLBackend)
		CHAI_METHOD(setProcAddress)
	END_CHAI()
}

//this could probably live in a different file
void registerServices()
{
	chai::kettle::PluginRegistry::Instance().Register("Renderer", "OpenGL", [] {
		return static_cast<void*>(new chai::brew::OpenGLBackend());
		});
}

CHAI_PLUGIN(OpenGL, "1.0.0", "chai::brew", "Renderer", registerServices)