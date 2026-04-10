#pragma once
#include <VulkanRendererExport.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/RenderFrame.h>
#include <Window/Window.h>
#include <Window/WindowSystem.h>

#include <vulkan/vulkan.h>

namespace chai::brew
{
    class VULKANRENDERER_EXPORT VulkanRenderer : public chai::brew::Renderer
    {
    public:
        VulkanRenderer();
        ~VulkanRenderer() override;

        bool initialize(std::unique_ptr<chai::RenderSurface> surface,
                        void* winProcAddress) override;
        void shutdown() override;

        // Main rendering entry point
        void executeCommands(const std::vector<chai::brew::RenderCommand>& commands) override;
    };
}

CHAI_PLUGIN_CLASS(VulkanPlugin)
