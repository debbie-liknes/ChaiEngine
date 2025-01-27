#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiGraphics/Window.h>

namespace CGraphics
{
	class CHAIGRAPHICS_EXPORT IRenderer
	{
	public:
		IRenderer();
		~IRenderer() = default;

		virtual void setProcAddress(void* address) {}
		virtual void renderFrame(Window* window) = 0;
	};
}
