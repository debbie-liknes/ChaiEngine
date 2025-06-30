#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/RenderCommandList.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::brew
{
		class CHAIGRAPHICS_EXPORT Renderer
		{
		public:
			Renderer();
			~Renderer() = default;

            virtual bool initialize(void* winProcAddress = nullptr) = 0;
            virtual void shutdown() = 0;
            virtual void executeCommands(const std::vector<RenderCommand>& commands) = 0;
		};
}