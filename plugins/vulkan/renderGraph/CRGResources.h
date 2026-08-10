#pragma once
#include "CRGDescriptors.h"

namespace chai::gfx
{
    class CRGResources
    {
    public:
        VkImageView view(CRGTextureHandle h, uint32_t mip = 0) const;
        VkImageView attachmentView(CRGTextureHandle h, uint32_t mip = 0) const;
        VkSampler sampler(CRGTextureHandle h) const;
        VkExtent2D extent(CRGTextureHandle h, uint32_t mip = 0) const;

    private:
        friend class ChaiRenderGraph;
        ChaiRenderGraph& graph_;

        CRGResources(ChaiRenderGraph& graph) : graph_(graph) {}
    };
}