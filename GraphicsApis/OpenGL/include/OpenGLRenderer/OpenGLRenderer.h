#pragma once
#include <OpenGLRendererExport.h>
#include <Engine/Renderer.h>

namespace CGraphics
{
	class RenderObject;
	enum ShaderStage;
	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
	public:
		OpenGLBackend();
		~OpenGLBackend() {}

		void setProcAddress(void* address) override;
		void renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros) override;
	private:
		int createShaderProgram(Core::CVector<int> shaders);
		int createShader(const char* source, CGraphics::ShaderStage stage);
	};
}