/**
 * @file CRGDescriptors.h
 */
#pragma once
#include "../commands/ImageTransition.h"
#include "../resources/RenderTargetView.h"
#include "../resources/VulkanRenderTarget.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    /**
     * @brief A generational handle object that is intetionally thin
     */
    struct CRGTextureHandle {
        uint32_t index = UINT32_MAX;
        uint32_t generation = 0;
        bool isValid() const { return index != UINT32_MAX; }
    };

    /**
     * @brief The type of texture in the render graph
     */
    enum class TextureType { Depth, Color2D, Cube };

    /**
     * @brief Used for the render graph to know what kind of render target to create
     */
    struct CRGTextureDesc {
        uint32_t width = 0, height = 0;
        VkFormat format = VK_FORMAT_UNDEFINED;
        uint32_t mipLevels = 1;
        TextureType type = TextureType::Color2D;
        VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;
    };

    /**
     * @brief Describes how textures will be used during pass execution
     */
    enum class CRGAccess { Read, Write };

    /**
     * @brief Helper struct to specify the texture and how it will be used
     */
    struct CRGTextureAccess {
        CRGTextureHandle handle;
        CRGAccess access;
        uint32_t mip = 0;
    };

    /**
     * @brief Structure owned by the render graph
     */
    struct CRGTexture {
        std::string name;
        CRGTextureDesc desc;

        bool isImported = false;
        RenderTarget target;                    // valid if isImported is false
        RenderTarget* importedTarget = nullptr; // valid if isImported is true

        std::vector<ImageState> mipStates; // tracks the state of each mip level

        uint32_t generation = 0;

        VkImage image() { return isImported ? importedTarget->image : target.image; }
    };

    /**
     * @brief Used to create a barrier plan for all textures owned by the render graph
     */
    struct CRGBarrier {
        VkImage image;
        ImageState from;
        ImageState to;
        uint32_t mip;
    };
} // namespace chai::gfx