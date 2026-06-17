/**
 * @file Material.h
 */
#pragma once
#include <Assets/TextureAsset.h>
#include <Common/GraphicsEnums.h>
#include <Handle.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace chai::gfx
{
    struct GpuMaterial {
        VkBuffer factorsBuffer = VK_NULL_HANDLE;
        VmaAllocation factorsAlloc = VK_NULL_HANDLE;
        VkDescriptorSet set = VK_NULL_HANDLE;

        Handle<Texture> baseColor{};
        Handle<Texture> metallicRoughness{};
        Handle<Texture> normal{};
        Handle<Texture> occlusion{};
        Handle<Texture> emissive{};

        AlphaMode alphaMode = AlphaMode::Opaque;
        bool doubleSided = false;
    };
} // namespace chai::gfx