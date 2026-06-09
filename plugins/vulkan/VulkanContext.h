/**
 * @file VulkanContext.h
 */
#pragma once
#include <cstdint>
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

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
    class VulkanContext
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

        void setupInstance(IWindow& window);
        void setupSurface(IWindow& window);
        void setupDevice();
        void setupQueues();
    };
} // namespace chai