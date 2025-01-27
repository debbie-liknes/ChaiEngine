#pragma once
#include <ChaiGraphicsExport.h>
#include <Core/MemoryTypes.h>
#include <Core/Containers.h>

namespace CGraphics
{
	class RenderObject;
	class Window;
	class CHAIGRAPHICS_EXPORT IRenderer
	{
	public:
		IRenderer();
		~IRenderer() = default;

		virtual void setProcAddress(void* address) {}
		virtual void renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros) = 0;
	};
}
