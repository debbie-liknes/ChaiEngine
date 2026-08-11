/**
 * @file CRGBuilder.h
 */
#pragma once
#include "CRGDescriptors.h"

namespace chai::gfx
{
    /**
     * @brief Used by the render graph to indicate which textures are required during execution.
     * When adding a pass, there are 2 phases: init and execute. The builder is used in the init to
     * allow the graph to build the correct dependecies at compile.
     */
    class CRGBuilder
    {
    public:
        /**
         * @brief Indicates the pass will use this texture handle to read during execution
         */
        CRGTextureHandle read(CRGTextureHandle h, uint32_t mip = 0);

        /**
         * @brief Indicates the pass will use this texture handle to write during execution
         */
        CRGTextureHandle write(CRGTextureHandle h, uint32_t mip = 0);

    private:
        friend class ChaiRenderGraph;
        explicit CRGBuilder(ChaiRenderGraph& renderGraph, uint32_t passIndex);
        ChaiRenderGraph& graph_;
        uint32_t passIndex_ = 0;
    };
} // namespace chai::gfx