#include <ChaiGraphicsExport.h>
#include <Core/MemoryTypes.h>
#include <Core/Containers.h>
#include <ChaiEngine/ViewData.h>
#include <ChaiEngine/Shader.h>

namespace chai
{
	class Window;
	namespace brew
	{
		class RenderObject;
		class CHAIGRAPHICS_EXPORT Renderer
		{
		public:
			Renderer();
			~Renderer() = default;

			virtual void setProcAddress(void* address) {}
			virtual void renderFrame(chai::Window* window, chai::CVector<chai::CSharedPtr<RenderObject>> ros, ViewData data) = 0;
			virtual std::shared_ptr<Shader> LoadOrGetShader(const std::string& path, ShaderStage stage) = 0;
		};
	}
}