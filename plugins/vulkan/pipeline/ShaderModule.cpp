#include "ShaderModule.h"

#include <Log.h>
#include <fstream>
#include <vector>

namespace chai::gfx
{
    VkShaderModule loadShaderModule(VkDevice device, const std::filesystem::path& spvPath)
    {
        std::ifstream file(spvPath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            CHAI_LOG_ERROR("Shader not found: {}", spvPath.string());
            return VK_NULL_HANDLE;
        }

        const size_t fileSize = static_cast<size_t>(file.tellg());

        // SPIR-V is a stream of 32-bit words. Read into a uint32_t buffer so the
        // pointer is correctly aligned for pCode.
        std::vector<uint32_t> code(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(code.data()), static_cast<std::streamsize>(fileSize));
        file.close();

        VkShaderModuleCreateInfo info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
        info.codeSize = fileSize; // in BYTES
        info.pCode = code.data(); // but the pointer is uint32_t*

        VkShaderModule module = VK_NULL_HANDLE;
        if (vkCreateShaderModule(device, &info, nullptr, &module) != VK_SUCCESS) {
            CHAI_LOG_ERROR("vkCreateShaderModule failed for: {}", spvPath.string());
            return VK_NULL_HANDLE;
        }
        return module;
    }
} // namespace chai::gfx