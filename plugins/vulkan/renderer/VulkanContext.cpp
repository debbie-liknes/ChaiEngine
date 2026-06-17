#include "VulkanContext.h"
#include <Window/Window.h>
#include <Log.h>
#include "../utils/VkCheck.h"

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
        setupDescriptors();
        setupQueues();
        setupAllocator();
        setupImmediate();
    }

    VulkanContext::~VulkanContext()
    {
        vkDestroyCommandPool(device_, immediatePool_, nullptr); // cmd buffer dies with it
        vkDestroyDescriptorSetLayout(device_, lightSetLayout_, nullptr);
        vkDestroyDescriptorSetLayout(device_, materialSetLayout_, nullptr);
        vkDestroyDescriptorSetLayout(device_, cameraSetLayout_, nullptr);
        vkDestroyDescriptorSetLayout(device_, environmentSetLayout_, nullptr);
        vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
        vkDestroyFence(device_, immediateFence_, nullptr);
        vmaDestroyAllocator(allocator_); // BEFORE the device allocator holds device memory
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
                            //.add_validation_feature_enable(
                            //   VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT)
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

        VkPhysicalDeviceFeatures required{};
        required.samplerAnisotropy = VK_TRUE;
        required.fillModeNonSolid = VK_TRUE;

        // use vkbootstrap to select a gpu.
        vkb::PhysicalDeviceSelector selector{vkbInstance_};
        vkbPhysicalDevice_ = selector.set_minimum_version(1, 3)
                                                 .set_required_features_13(features)
                                                 .set_required_features_12(features12)
                                                 .set_required_features(required)
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

    void VulkanContext::setupAllocator()
    {
        VmaVulkanFunctions vkFuncs{};
        vkFuncs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vkFuncs.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocInfo{};
        allocInfo.physicalDevice = physicalDevice_;
        allocInfo.device = device_;
        allocInfo.instance = instance_;
        allocInfo.pVulkanFunctions = &vkFuncs;
        allocInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        VK_CHECK(vmaCreateAllocator(&allocInfo, &allocator_));
    }

    void VulkanContext::setupImmediate()
    {
        VkCommandPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex =
            graphicsFamily_; // or a dedicated transfer family if you have one
        vkCreateCommandPool(device_, &poolInfo, nullptr, &immediatePool_);

        VkCommandBufferAllocateInfo cmdInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
        cmdInfo.commandPool = immediatePool_;
        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdInfo.commandBufferCount = 1;
        vkAllocateCommandBuffers(device_, &cmdInfo, &immediateCmd_);

        VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        // start this unsignaled
        vkCreateFence(device_, &fenceInfo, nullptr, &immediateFence_);
    }

    void VulkanContext::setupDescriptors()
    {
        //i will outgrow this budget. Will need a pool of pools
        VkDescriptorPoolSize poolSizes[] = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 512},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2048}
        };

        VkDescriptorPoolCreateInfo poolInfo{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        poolInfo.flags = 0;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.poolSizeCount = uint32_t(std::size(poolSizes));
        poolInfo.maxSets = 512;
        VK_CHECK(vkCreateDescriptorPool(device_, &poolInfo, nullptr, &descriptorPool_));

        //cameras
        VkDescriptorSetLayoutBinding camBinding{};
        camBinding.binding = 0;
        camBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        camBinding.descriptorCount = 1;
        camBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo camLayout{
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        camLayout.bindingCount = 1;
        camLayout.pBindings = &camBinding;
        VK_CHECK(vkCreateDescriptorSetLayout(device_, &camLayout, nullptr, &cameraSetLayout_));

        //materials
        VkDescriptorSetLayoutBinding matBindings[6]{};
        matBindings[0].binding = 0;
        matBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        matBindings[0].descriptorCount = 1;
        matBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        for (uint32_t i = 1; i < 6; ++i) {
            matBindings[i].binding = i;
            matBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            matBindings[i].descriptorCount = 1;
            matBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        VkDescriptorSetLayoutCreateInfo matLayout{
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        matLayout.bindingCount = 6;
        matLayout.pBindings = matBindings;
        VK_CHECK(vkCreateDescriptorSetLayout(device_, &matLayout, nullptr, &materialSetLayout_));

        // lights
        VkDescriptorSetLayoutBinding lightBinding{};
        lightBinding.binding = 0;
        lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightBinding.descriptorCount = 1;
        lightBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo lightLayout{
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        lightLayout.bindingCount = 1;
        lightLayout.pBindings = &lightBinding;
        VK_CHECK(vkCreateDescriptorSetLayout(device_, &lightLayout, nullptr, &lightSetLayout_));

        //environment (skybox)
        VkDescriptorSetLayoutBinding envBindings[2]{};
        envBindings[0].binding = 0;
        envBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        envBindings[0].descriptorCount = 1;
        envBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        envBindings[1].binding = 1;
        envBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        envBindings[1].descriptorCount = 1;
        envBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo envLayout{
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        envLayout.bindingCount = 2;
        envLayout.pBindings = envBindings;
        VK_CHECK(vkCreateDescriptorSetLayout(
            device_, &envLayout, nullptr, &environmentSetLayout_));
    }
} // namespace chai