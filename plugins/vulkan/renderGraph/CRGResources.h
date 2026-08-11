/**
 * @file CRGResources.h
 */
#pragma once
#include "CRGDescriptors.h"

namespace chai::gfx
{
    class ChaiRenderGraph;

    /**
     * @brief Only used by the execute function on the render graph to allow access to texture
     * handles owned by the graph. Graph owned handles cannot be used to retrieve these outside of
     * the context of the execute function.
     * 
     * @note Do not attempt to save acquired resources for later use. Use them only inside execution
     */
    class CRGResources
    {
    public:
        /**
         * @brief Resource's full range sampling view. This is the view bound to a descriptor set to read the rexture into a shader.
         */
        VkImageView view(CRGTextureHandle h, uint32_t mip = 0) const;

        /**
         * @brief A single mip view
         */
        VkImageView attachmentView(CRGTextureHandle h, uint32_t mip = 0) const;

        /**
         * @brief Texture Sampler
         */
        VkSampler sampler(CRGTextureHandle h) const;

        /**
         * @brief Texture size
         */
        VkExtent2D extent(CRGTextureHandle h, uint32_t mip = 0) const;

    private:
        friend class ChaiRenderGraph;
        ChaiRenderGraph& graph_;

        CRGResources(ChaiRenderGraph& graph) : graph_(graph) {}
    };
} // namespace chai::gfx
