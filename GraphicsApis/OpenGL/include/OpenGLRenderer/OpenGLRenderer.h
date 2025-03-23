#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/ViewData.h>

namespace chai_graphics
{
	class RenderObject;
	enum ShaderStage;
	class GlPipelineState;

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
	public:
		OpenGLBackend();
		~OpenGLBackend() {}

		void setProcAddress(void* address) override;
		void renderFrame(chai::Window* window, chai::CVector<chai::CSharedPtr<RenderObject>> ros, ViewData data) override;
	private:
		int createShaderProgram(chai::CVector<int> shaders);
		int createShader(const char* source, ShaderStage stage);

		//Core::CSharedPtr<GlPipelineState> m_pipelineState;
	};
}