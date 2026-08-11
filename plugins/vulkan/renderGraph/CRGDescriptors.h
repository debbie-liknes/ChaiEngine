#pragma once
#include "../commands/ImageTransition.h"
#include "../resources/RenderTargetView.h"
#include "../resources/VulkanRenderTarget.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    struct CRGTextureHandle 
    {
        uint32_t index = UINT32_MAX;
        uint32_t generation = 0;
        bool isValid() const { return index != UINT32_MAX; }
    };

    enum class TextureType { Depth, Color2D, Cube };

    struct CRGTextureDesc 
    {
        uint32_t width = 0, height = 0;
        VkFormat format = VK_FORMAT_UNDEFINED;
        uint32_t mipLevels = 1;
        TextureType type = TextureType::Color2D;
    };

    enum class CRGAccess { Read, Write };

    struct CRGTextureAccess 
    {
        CRGTextureHandle handle;
        CRGAccess access;
        uint32_t mip = 0;
    };

    struct CRGTexture 
    {
        std::string name;
        CRGTextureDesc desc;

        bool isImported = false;
        RenderTarget target;             // valid if isImported is false
        RenderTarget* importedTarget = nullptr; // valid if isImported is true

        std::vector<ImageState> mipStates; // tracks the state of each mip level

        uint32_t generation = 0;

        VkImage image() { return isImported ? importedTarget->image : target.image; }
    };

    struct CRGBarrier {
        VkImage image;
        ImageState from;
        ImageState to;
        uint32_t mip;
    };
} // namespace chai::gfx