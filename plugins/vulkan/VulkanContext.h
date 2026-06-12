/**
 * @file VulkanContext.h
 */
#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"
#include <vk_mem_alloc.h>
#include <Rendering/IRenderer.h>

namespace chai
{
    class IWindow;
}

namespace chai::gfx
{
    /**
     * @brief A structure to store long lived Vulkan handles. Things created once
     * that live for the renderer's lifetime. Instance, surface, devices, queues.
     * Uses vk-bootstrap for the boilerplate code, but provides getters for the 
     * Vulkan objects
     */
    class VulkanContext : public IRenderDevice
    {
    public:
        explicit VulkanContext(IWindow& window); // creates everything below
        ~VulkanContext();

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

        VkInstance instance() const { return instance_; }
        VkSurfaceKHR surface() const { return surface_; }
        VkPhysicalDevice physicalDevice() const { return physicalDevice_; }
        VkDevice device() const { return device_; }
        VkQueue graphicsQueue() const { return graphicsQueue_; }
        VkQueue presentQueue() const { return presentQueue_; }
        uint32_t graphicsFamily() const { return graphicsFamily_; }
        uint32_t presentFamily() const { return presentFamily_; }
        VmaAllocator allocator() const { return allocator_; }

        VkCommandPool immediatePool() { return immediatePool_; }
        VkCommandBuffer immediateCmd() const { return immediateCmd_; }
        VkFence immediateFence() const { return immediateFence_; }

        VkDescriptorSetLayout cameraSetLayout() const { return cameraSetLayout_; }
        VkDescriptorSetLayout materialSetLayout() const { return materialSetLayout_; }
        VkDescriptorPool descriptorPool() const { return descriptorPool_; }

    private:
        vkb::Instance vkbInstance_;
        VkInstance instance_ = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
        VkSurfaceKHR surface_ = VK_NULL_HANDLE;

        vkb::Device vkbDevice_;
        vkb::PhysicalDevice vkbPhysicalDevice_;
        VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
        VkDevice device_ = VK_NULL_HANDLE;

        VkQueue graphicsQueue_ = VK_NULL_HANDLE;
        VkQueue presentQueue_ = VK_NULL_HANDLE;
        uint32_t graphicsFamily_ = 0;
        uint32_t presentFamily_ = 0;

        VmaAllocator allocator_;

        VkCommandPool immediatePool_ = VK_NULL_HANDLE;
        VkCommandBuffer immediateCmd_ = VK_NULL_HANDLE;
        VkFence immediateFence_ = VK_NULL_HANDLE;

        //descriptor stuff - may move this later and keep this class pure
        VkDescriptorPool descriptorPool_ = VK_NULL_HANDLE;
        VkDescriptorSetLayout cameraSetLayout_ = VK_NULL_HANDLE;
        VkDescriptorSetLayout materialSetLayout_ = VK_NULL_HANDLE;

        void setupInstance(IWindow& window);
        void setupSurface(IWindow& window);
        void setupDevice();
        void setupQueues();
        void setupAllocator();
        void setupImmediate();
        void setupDescriptors();
    };
} // namespace chai