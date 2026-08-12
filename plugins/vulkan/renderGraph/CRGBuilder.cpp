#include "CRGBuilder.h"
#include "ChaiRenderGraph.h"

namespace chai::gfx
{
    CRGBuilder::CRGBuilder(ChaiRenderGraph& renderGraph, uint32_t passIndex)
        : graph_(renderGraph), passIndex_(passIndex)
    {

    }

    CRGTextureHandle CRGBuilder::read(CRGTextureHandle h, uint32_t mip)
    {
        graph_.passes_[passIndex_]->accesses.push_back({h, CRGAccess::Read, mip});
        return h;
    }

    CRGTextureHandle CRGBuilder::write(CRGTextureHandle h, uint32_t mip) 
    {
        graph_.passes_[passIndex_]->accesses.push_back({h, CRGAccess::Write, mip});
        return h;
    }
}