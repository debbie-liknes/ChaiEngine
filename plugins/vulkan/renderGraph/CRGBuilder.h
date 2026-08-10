#pragma once
#include "CRGDescriptors.h"

namespace chai::gfx
{
    class CRGBuilder
    {
    public:
        CRGTextureHandle read(CRGTextureHandle h, uint32_t mip = 0);
        CRGTextureHandle write(CRGTextureHandle h, uint32_t mip = 0);

    private:
        friend class ChaiRenderGraph;
        explicit CRGBuilder(ChaiRenderGraph& renderGraph, uint32_t passIndex);
        ChaiRenderGraph& graph_;
        uint32_t passIndex_ = 0;
    };
} // namespace chai::gfx