#pragma once
#include <ChaiEngine/RenderCommandList.h>

namespace chai::brew
{
    struct RenderFrame 
    {
        std::vector<RenderCommand> commands;
        uint64_t frameNumber;

        RenderFrame() : frameNumber(0) {}
        RenderFrame(std::vector<RenderCommand>&& cmds, uint64_t num)
            : commands(std::move(cmds)), frameNumber(num)
        {
        }
    };
}