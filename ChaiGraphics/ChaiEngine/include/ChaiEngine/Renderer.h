#pragma once
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
			virtual void renderFrame(const RenderFrame& frame) = 0;
			virtual std::shared_ptr<ITextureBackend> createTexture2D(const uint8_t* data, uint32_t width, uint32_t height) = 0;
		};
	}
}