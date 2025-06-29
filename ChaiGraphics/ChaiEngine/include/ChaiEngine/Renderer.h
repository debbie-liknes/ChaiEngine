#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/ViewData.h>
#include <ChaiEngine/Shader.h>
#include <ChaiEngine/RenderCommandList.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

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

            virtual bool initialize(void* winProcAddress = nullptr) = 0;
            virtual void shutdown() = 0;
            virtual void executeCommands(const std::vector<RenderCommand>& commands) = 0;

			//virtual std::unique_ptr<RenderCommandList> createCommandList() = 0;
			//virtual void executeCommandList(RenderCommandList& cmdList) = 0;
			//virtual void present(Window& window) = 0;
			//virtual void setProcAddress(void* address) {}
			//virtual void renderFrame(const RenderFrame& frame) = 0;
			//virtual std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) = 0;
		};
	}
}