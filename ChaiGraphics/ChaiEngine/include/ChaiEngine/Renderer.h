#include <ChaiGraphicsExport.h>
#include <Core/MemoryTypes.h>
#include <Core/Containers.h>
#include <ChaiEngine/ViewData.h>

namespace CGraphics
{
	class Window;
}

namespace chai_graphics
{
	class RenderObject;
	class CHAIGRAPHICS_EXPORT Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		virtual void setProcAddress(void* address) {}
		virtual void renderFrame(CGraphics::Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros, chai_graphics::ViewData data) = 0;
	};
}