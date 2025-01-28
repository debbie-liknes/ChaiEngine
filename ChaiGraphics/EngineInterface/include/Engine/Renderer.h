#include <ChaiApiExport.h>
#include <Core/MemoryTypes.h>
#include <Core/Containers.h>

namespace CGraphics
{
	class RenderObject;
	class Window;
	class CHAIAPI_EXPORT Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		virtual void setProcAddress(void* address) {}
		virtual void renderFrame(Window* window, Core::CVector<Core::CSharedPtr<RenderObject>> ros) = 0;
	};
}