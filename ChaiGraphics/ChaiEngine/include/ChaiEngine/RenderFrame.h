#pragma once
#include <ChaiEngine/RenderCommandList.h>

namespace chai::brew
{
    struct CameraFrameData
    {
        float nearPlane;
        float farPlane;
    };

    struct RenderFrame 
    {
        std::vector<RenderCommand> commands;
        uint64_t frameNumber;
        float nearPlane{0.1f};
        float farPlane{1000.f};

        RenderFrame() : frameNumber(0) {}
        RenderFrame(std::vector<RenderCommand>&& cmds, uint64_t num)
            : commands(std::move(cmds)), frameNumber(num)
        {
        }
        RenderFrame(const std::vector<RenderCommand>& vector, uint64_t uint64, float near_plane, float far_plane)
        {

        }
    };
}