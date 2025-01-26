#pragma once
#include <OpenGLRendererExport.h>
#include <ChaiGraphics/IRenderer.h>

namespace CGraphics
{
	class OPENGLRENDERER_EXPORT OpenGLBackend : public IRenderer
	{
	public:
		OpenGLBackend() = default;
		~OpenGLBackend() {}

		void setProcAddress(void* address) override;
	private:

	};
}