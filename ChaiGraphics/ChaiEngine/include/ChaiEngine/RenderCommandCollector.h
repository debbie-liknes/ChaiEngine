#pragma once
#include <ChaiEngine/RenderCommandList.h>
#include <Scene/ICamera.h>
#include <Graphics/Viewport.h>

namespace chai::brew
{
    class RenderCommandCollector
    {
    public:
        void submit(RenderCommand& cmd);
        const std::vector<RenderCommand>& getCommands() const;

        void setCamera(ICamera* cam);
        void setViewport(IViewport* vp);

    private:
        std::vector<RenderCommand> commands;
        ICamera* camera = nullptr;
        IViewport* viewport = nullptr;
    };
}