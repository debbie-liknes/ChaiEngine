#include "VulkanRenderer.h"

namespace chai::brew
{
    VulkanRenderer::VulkanRenderer()
    {
        // Constructor implementation
    }
    VulkanRenderer::~VulkanRenderer()
    {
        // Destructor implementation
    }

    bool VulkanRenderer::initialize(std::unique_ptr<chai::RenderSurface> surface,
                                  void* winProcAddress)
    {
        uint32_t version = 0;
        vkEnumerateInstanceVersion(&version);

        std::cout << "Vulkan version: " << VK_VERSION_MAJOR(version) << "."
                  << VK_VERSION_MINOR(version) << "." << VK_VERSION_PATCH(version) << std::endl;

        return false;
    }
    void VulkanRenderer::shutdown() {}

    // Main rendering entry point
    void VulkanRenderer::executeCommands(const std::vector<chai::brew::RenderCommand>& commands) {}
}

CHAI_PLUGIN_SERVICES(VulkanPlugin)
{
    CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::VulkanRenderer, "Renderer")
}

CHAI_DEFINE_PLUGIN_ENTRY(VulkanPlugin, "VulkanRenderer", "1.0.0", VULKANRENDERER_EXPORT)