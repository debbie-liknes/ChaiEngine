#pragma once
#include "RenderFrame.h"

#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/RenderCommandList.h>

#include <ChaiGraphicsExport.h>

namespace chai
{
    class RenderSurface;
}

namespace chai::brew
{
    class CHAIGRAPHICS_EXPORT Renderer
    {
    public:
        Renderer();
        virtual ~Renderer() = default;

        virtual bool initialize(std::unique_ptr<RenderSurface> surface, void* winProcAddress = nullptr) = 0;
        virtual void shutdown() = 0;
        virtual void executeCommands(const std::vector<RenderCommand>& commands) = 0;
    };
}