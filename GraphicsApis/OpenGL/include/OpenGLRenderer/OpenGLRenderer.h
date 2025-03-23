#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/ViewData.h>

namespace CGraphics
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
		void renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros, chai_graphics::ViewData data) override;
	private:
		int createShaderProgram(Core::CVector<int> shaders);
		int createShader(const char* source, CGraphics::ShaderStage stage);

		//Core::CSharedPtr<GlPipelineState> m_pipelineState;
	};
}