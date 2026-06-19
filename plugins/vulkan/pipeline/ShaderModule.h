/**
 * @file ShaderModule.h
 * @brief Free function to read a .spv file and create a shader module
 */
#pragma once
#include <filesystem>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    VkShaderModule loadShaderModule(VkDevice device, const std::filesystem::path& spvPath);
} // namespace chai::gfx