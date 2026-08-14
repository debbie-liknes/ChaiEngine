#pragma once
#include "../renderer/VulkanContext.h"
#include "PipelineBuilder.h"
#include "PipelineHelpers.h"
#include "ShaderModule.h"

#include <OS/SystemPaths.h>
#include <Log.h>
#include <filesystem>
#include <string_view>

namespace chai::gfx
{
    template <typename ConfigureFn>
    VkPipeline loadPipeline(VulkanContext& ctx,
                            const std::filesystem::path& vertPath,
                            const std::filesystem::path& fragPath,
                            VkPipelineLayout layout,
                            ConfigureFn&& configure)
    {
        VkShaderModule vert = loadShaderModule(ctx.device(), vertPath);
        VkShaderModule frag = loadShaderModule(ctx.device(), fragPath);
        if (vert == VK_NULL_HANDLE || frag == VK_NULL_HANDLE) {
            CHAI_LOG_CRITICAL(
                "loadPipeline: shader load failed ({}, {})", vertPath.string(), fragPath.string());
            if (vert)
                vkDestroyShaderModule(ctx.device(), vert, nullptr);
            if (frag)
                vkDestroyShaderModule(ctx.device(), frag, nullptr);
            return VK_NULL_HANDLE;
        }

        PipelineBuilder builder;
        builder.setShaders(vert, frag);
        configure(builder); // caller sets state on the primed builder
        VkPipeline pipe = builder.build(ctx.device(), layout);

        vkDestroyShaderModule(ctx.device(), vert, nullptr);
        vkDestroyShaderModule(ctx.device(), frag, nullptr);
        return pipe;
    }

    template <typename ConfigureFn>
    VkPipeline loadPipelineByName(VulkanContext& ctx,
                                  std::string_view vertName,
                                  std::string_view fragName,
                                  VkPipelineLayout layout,
                                  ConfigureFn&& configure)
    {
        const auto dir = executableDir() / "shaders";
        return loadPipeline(
            ctx, dir / vertName, dir / fragName, layout, std::forward<ConfigureFn>(configure));
    }
} // namespace chai::gfx