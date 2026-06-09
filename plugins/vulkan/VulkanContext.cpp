#include "VulkanContext.h"
#include <Window/Window.h>
#include <Log.h>

namespace chai
{
    class IWindow;
}

namespace chai::gfx
{
    VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                  VkDebugUtilsMessageTypeFlagsEXT type,
                  const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                  void* userData)
    {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                CHAI_LOG_TRACE("[Vulkan] {}", callbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                CHAI_LOG_INFO("[Vulkan] {}", callbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                CHAI_LOG_WARN("[Vulkan] {}", callbackData->pMessage);
                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                CHAI_LOG_ERROR("[Vulkan] {}", callbackData->pMessage);
                break;
        }

        return VK_FALSE;
    }

    VulkanContext::VulkanContext(IWindow& window)
    {
        setupInstance(window);
        setupSurface(window);
        setupDevice();
        setupQueues();
    }

    VulkanContext::~VulkanContext()
    {
        vkDestroyDevice(device_, nullptr);
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkb::destroy_debug_utils_messenger(instance_, debugMessenger_);
        vkDestroyInstance(instance_, nullptr);
    }

    void VulkanContext::setupInstance(IWindow& window)
    {
        vkb::InstanceBuilder builder;

        auto windowExtensions = window.getExtensions();

        auto instRet = builder.set_app_name("")
                            .request_validation_layers(true)
                            .set_debug_callback(DebugCallback)
                            .enable_extensions(windowExtensions)
                            .require_api_version(1, 3, 0)
                            .build();

        vkbInstance_ = instRet.value();

        // grab the instance and debug messenger
        instance_ = vkbInstance_.instance;
        debugMessenger_ = vkbInstance_.debug_messenger;
    }

    void VulkanContext::setupSurface(IWindow& window)
    {
        void* surface = window.createSurface_Vulkan(&instance_);

        if (!surface) {
            CHAI_LOG_CRITICAL("Vulkan surface is null.");
            return;
        }

        surface_ = static_cast<VkSurfaceKHR>(surface);

        if (!surface_) {
            CHAI_LOG_CRITICAL("Surface provided is not type VkSurfaceKHR");
            return;
        }
    }

    void VulkanContext::setupDevice()
    {
        // vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
        features.dynamicRendering = true;
        features.synchronization2 = true;

        // vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        // use vkbootstrap to select a gpu.
        // We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct
        // features
        vkb::PhysicalDeviceSelector selector{vkbInstance_};
        vkbPhysicalDevice_ = selector.set_minimum_version(1, 3)
                                                 .set_required_features_13(features)
                                                 .set_required_features_12(features12)
                                                 .set_surface(surface_)
                                                 .select()
                                                 .value();

        // create the final vulkan device
        vkb::DeviceBuilder deviceBuilder{vkbPhysicalDevice_};

        vkbDevice_ = deviceBuilder.build().value();

        device_ = vkbDevice_.device;
        physicalDevice_ = vkbPhysicalDevice_.physical_device;
    }

    void VulkanContext::setupQueues()
    {
        graphicsQueue_ = vkbDevice_.get_queue(vkb::QueueType::graphics).value();
        graphicsFamily_ = vkbDevice_.get_queue_index(vkb::QueueType::graphics).value();
        presentQueue_ = vkbDevice_.get_queue(vkb::QueueType::present).value();
        presentFamily_ = vkbDevice_.get_queue_index(vkb::QueueType::present).value();
    }
} // namespace chai