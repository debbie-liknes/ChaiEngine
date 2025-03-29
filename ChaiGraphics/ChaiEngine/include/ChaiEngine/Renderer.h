#include <ChaiGraphicsExport.h>
#include <Core/MemoryTypes.h>
#include <Core/Containers.h>
#include <ChaiEngine/ViewData.h>
#include <ChaiEngine/Shader.h>
#include <Core/TextureBackend.h>
#include <ChaiEngine/RenderFrame.h>

namespace chai
{
	class Window;
	namespace brew
	{
		class Renderable;
		class CHAIGRAPHICS_EXPORT Renderer
		{
		public:
			Renderer();
			~Renderer() = default;

			virtual void setProcAddress(void* address) {}
			//virtual void renderFrame(chai::Window* window, chai::CVector<chai::CSharedPtr<Renderable>> ros, ViewData data) = 0;
			virtual void renderFrame(const RenderFrame& frame) = 0;
			virtual std::shared_ptr<Shader> LoadOrGetShader(const std::string& path, ShaderStage stage) = 0;
			virtual std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) = 0;
		};
	}
}