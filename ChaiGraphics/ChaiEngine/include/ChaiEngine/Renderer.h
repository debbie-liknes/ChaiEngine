#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/ViewData.h>
#include <ChaiEngine/Shader.h>
#include <Core/TextureBackend.h>
#include <ChaiEngine/RenderFrame.h>
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

            // Resource factories (implemented by concrete plugins)
            virtual std::shared_ptr<IMesh> createMesh(
                const std::vector<Vertex>& vertices,
                const std::vector<uint32_t>& indices = {}
            ) = 0;

            virtual std::shared_ptr<IMaterial> createMaterial(
                const std::string& vertexShader,
                const std::string& fragmentShader
            ) = 0;

            // Texture management
            virtual uint32_t createTexture(
                const void* data,
                int width, int height,
                int channels
            ) = 0;

			//virtual std::unique_ptr<RenderCommandList> createCommandList() = 0;
			//virtual void executeCommandList(RenderCommandList& cmdList) = 0;
			//virtual void present(Window& window) = 0;
			//virtual void setProcAddress(void* address) {}
			//virtual void renderFrame(const RenderFrame& frame) = 0;
			//virtual std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) = 0;
		};
	}
}