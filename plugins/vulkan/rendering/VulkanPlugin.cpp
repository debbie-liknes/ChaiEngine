/**
 * @file VulkanPlugin.h
 * @brief Plugin setup and registration of services
 */
#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <Window/Window.h>
#include <memory>
#include "VulkanRenderer.h"
#include "../Mesh.h"
#include "../TextureFactory.h"

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

            vulkCtx_ = std::make_shared<VulkanContext>(*window);
            resources_ = std::make_shared<GpuResources>(*vulkCtx_);
            meshRegistry_ =
                std::make_shared<MeshRegistry>(resources_->factory(), resources_->graveyard());
            texFactory_ = std::make_shared<TextureFactory>(vulkCtx_.get());
            texRegistry_ = std::make_shared<TextureRegistry>(
                *texFactory_, resources_->graveyard(), &ctx.services);
            renderer_ =
                std::make_shared<VulkanRenderer>(*window, meshRegistry_->cache(), texRegistry_->cache(), *vulkCtx_);


            //register services, but make sure to UN-register them on unload
            ctx.services.provide<AssetCache<Mesh>>(meshRegistry_->cache());
            ctx.services.provide<IRenderer>(renderer_);
            ctx.services.provide<IMeshRegistry>(meshRegistry_);
            ctx.services.provide<IRenderDevice>(vulkCtx_);
            ctx.services.provide<ITextureRegistry>(texRegistry_);
            ctx.services.provide<TextureFactory>(texFactory_);

            CHAI_LOG_INFO("Renderer service provided");
        }

        void onUnload(PluginContext& ctx) override
        {
            if (renderer_)
                renderer_->waitIdle(); // probably unnecessary here

            //remove services
            ctx.services.remove<TextureFactory>();
            ctx.services.remove<ITextureRegistry>();
            ctx.services.remove<IRenderDevice>();
            ctx.services.remove<IMeshRegistry>();
            ctx.services.remove<IRenderer>();
            ctx.services.remove<AssetCache<Mesh>>();

            //release all our resources and pointers
            meshRegistry_->cache()->releaseAll();
            texRegistry_->cache()->releaseAll();

            renderer_.reset();
            resources_.reset();
            meshRegistry_.reset();
            texRegistry_.reset();
            texFactory_.reset();
            vulkCtx_.reset();

            CHAI_LOG_INFO("Renderer removed");
        }

    private:
        std::shared_ptr<VulkanContext> vulkCtx_;
        std::shared_ptr<GpuResources> resources_;
        std::shared_ptr<VulkanRenderer> renderer_;
        std::shared_ptr<MeshRegistry> meshRegistry_;
        std::shared_ptr<TextureFactory> texFactory_;
        std::shared_ptr<TextureRegistry> texRegistry_;
    };

    CHAI_PLUGIN(VulkanPlugin);
} // namespace chai