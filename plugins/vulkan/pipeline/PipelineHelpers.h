#pragma once
#include "../VulkanCommon.h"
#include "../renderer/VulkanContext.h"
#include "PipelineBuilder.h"
#include "PipelineHelpers.h"
#include "ShaderModule.h"

#include <Runtime/SystemPaths.h>
#include <Log.h>
#include <filesystem>
#include <string_view>

#include "ShaderCompiler.h"

namespace chai::gfx
{
    inline VkShaderModule createShaderMod(VkDevice device, const std::vector<uint32_t>& spirv)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirv.size() * sizeof(uint32_t); // byte size, not element count
        createInfo.pCode = spirv.data();

        VkShaderModule module;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS) {
            CHAI_LOG_ERROR("Failed to create shader module");
            return VK_NULL_HANDLE;
        }
        return module;
    }

    template <typename ConfigureFn>
    VkPipeline loadPipeline(VulkanContext& ctx,
                            const std::filesystem::path& vertPath,
                            const std::filesystem::path& fragPath,
                            VkPipelineLayout layout,
                            ConfigureFn&& configure)
    {
        ShaderCompiler compiler;
        ShaderCompiler::CompileResult vertResult = compiler.compile(vertPath, ShaderStage::Vertex);
        ShaderCompiler::CompileResult fragResult = compiler.compile(fragPath, ShaderStage::Fragment);

        if (!vertResult.success) {
            CHAI_LOG_ERROR("Failed to compile shader {} with error {}",
                           vertPath.string(),
                           vertResult.errorLog);
            return VK_NULL_HANDLE;
        } else if (!fragResult.success) {
            CHAI_LOG_ERROR("Failed to compile shader {} with error {}",
                           fragPath.string(),
                           fragResult.errorLog);
            return VK_NULL_HANDLE;
        }

        VkShaderModule vert = createShaderMod(ctx.device(), vertResult.spirv);
        VkShaderModule frag = createShaderMod(ctx.device(), fragResult.spirv);

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
        const auto dir = shaderDir();
        return loadPipeline(
            ctx, dir / vertName, dir / fragName, layout, std::forward<ConfigureFn>(configure));
    }
} // namespace chai::gfx