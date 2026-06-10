#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <Window/Window.h>
#include <memory>
#include "VulkanRenderer.h"

namespace chai::gfx
{
    class VulkanPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Renderer(Vulkan)"; }

        void onLoad(PluginContext& ctx) override
        {
            auto window = ctx.services.tryResolve<chai::IWindow>();
            if (!window) {
                CHAI_LOG_CRITICAL("Renderer requires IWindow; load the window plugin first");
                return;
            }

            services_ = &ctx.services;
            renderer_ = std::make_shared<VulkanRenderer>(*window, services_);
            ctx.services.provide<IRenderer>(renderer_);
            CHAI_LOG_INFO("Renderer service provided");

        }

        void onUnload(PluginContext& ctx) override
        {
            if (renderer_)
                renderer_->waitIdle(); // make sure the GPU is idle before teardown
            ctx.services.remove<IRenderer>();
            renderer_.reset();
            services_ = nullptr;
            CHAI_LOG_INFO("Renderer removed");

        }

    private:
        std::shared_ptr<VulkanRenderer> renderer_;
        ServiceLocator* services_;
    };

    CHAI_PLUGIN(VulkanPlugin);
} // namespace chai