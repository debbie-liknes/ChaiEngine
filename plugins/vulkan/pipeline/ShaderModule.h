#pragma once
#include <filesystem>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    // Reads a compiled SPIR-V (.spv) file and creates a VkShaderModule.
    // Returns VK_NULL_HANDLE on failure (logged). The returned module is only
    // needed during pipeline creation — destroy it right after build().
    VkShaderModule loadShaderModule(VkDevice device, const std::filesystem::path& spvPath);
} // namespace chai::gfx