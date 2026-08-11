#include "CRGResources.h"
#include "ChaiRenderGraph.h"

namespace chai::gfx
{
    //TODO: These functions are the same and idk what to do about it
    VkImageView CRGResources::view(CRGTextureHandle h, uint32_t mip) const
    {
        return graph_.textures_[h.index].isImported
                   ? graph_.textures_[h.index].importedTarget->renderView(mip)
                   : graph_.textures_[h.index].target.renderView(mip);
    }

    VkImageView CRGResources::attachmentView(CRGTextureHandle h, uint32_t mip) const
    {
        return graph_.textures_[h.index].isImported
                   ? graph_.textures_[h.index].importedTarget->renderView(mip)
                   : graph_.textures_[h.index].target.renderView(mip);
    }

    VkSampler CRGResources::sampler(CRGTextureHandle h) const
    {
        return graph_.textures_[h.index].isImported ? graph_.ctx_.linearSampler()
                   : graph_.textures_[h.index].target.sampler;
    }

    VkExtent2D CRGResources::extent(CRGTextureHandle h, uint32_t mip) const
    {
        return graph_.textures_[h.index].isImported
                   ? graph_.textures_[h.index].importedTarget->extent
                   : VkExtent2D{graph_.textures_[h.index].target.extent.width >> mip,
                                graph_.textures_[h.index].target.extent.height >> mip};

    }
}