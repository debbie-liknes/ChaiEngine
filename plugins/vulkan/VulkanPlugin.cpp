/**
 * @file VulkanPlugin.h
 * @brief Plugin setup and registration of services
 */
#include "VulkanPluginStatsPanel.h"
#include "registries/ViewportRegistry.h"
#include "renderer/VulkanRenderer.h"
#include "resources/MaterialFactory.h"
#include "resources/MaterialRegistry.h"
#include "resources/Mesh.h"
#include "resources/MeshFactory.h"
#include "resources/MeshRegistry.h"
#include "resources/ModelRegistry.h"
#include "resources/TextureFactory.h"
#include "resources/TextureRegistry.h"

#include <Assets/IMeshRegistry.h>
#include <EditorUI/ActionManager.h>
#include <EditorUI/PanelRegistry.h>
#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <Window/Window.h>
#include <memory>

namespace chai::gfx
{
    class VulkanPlugin : public IPlugin
    {
    public:
        const char* name() const override { return "Renderer(Vulkan)"; }

        ServiceList requiredServices() const override
        {
            return {typeid(IWindow),
                    typeid(ITextureLoader),
                    typeid(ui::PanelRegistry),
                    typeid(ui::ActionManager)};
        }
        ServiceList providedServices() const override
        {
            return {typeid(AssetCache<Mesh>),
                    typeid(IRenderer),
                    typeid(IMeshRegistry),
                    typeid(ITextureRegistry),
                    typeid(TextureFactory),
                    typeid(IModelRegistry),
                    typeid(IMeshRegistry),
                    typeid(IViewportRegistry)};
        }

        [[nodiscard]] bool onLoad(PluginContext& ctx) override
        {
            auto window = ctx.services.tryResolve<IWindow>();
            if (!window) {
                CHAI_LOG_CRITICAL("Renderer requires IWindow; load the window plugin first");
                return false;
            }

            auto textureLoader = ctx.services.tryResolve<ITextureLoader>();
            if (!textureLoader) {
                CHAI_LOG_CRITICAL(
                    "Renderer requires ITextureLoader; load the texture loader plugin first");
                return false;
            }

            vulkCtx_ = std::make_shared<VulkanContext>(*window);
            resources_ = std::make_shared<GpuResources>(*vulkCtx_);
            meshRegistry_ =
                std::make_shared<MeshRegistry>(resources_->factory(), resources_->graveyard());
            texFactory_ = std::make_shared<TextureFactory>(vulkCtx_.get());
            texRegistry_ = std::make_shared<TextureRegistry>(
                *texFactory_, resources_->graveyard(), &ctx.services);
            matRegistry_ = std::make_shared<MaterialRegistry>(
                *vulkCtx_, texRegistry_->cache(), resources_->graveyard());
            modelRegistry_ = std::make_shared<ModelRegistry>(
                *meshRegistry_, *texRegistry_, *matRegistry_, &ctx.services);
            viewportRegistry_ = std::make_shared<ViewportRegistry>(*vulkCtx_);
            renderer_ = std::make_shared<VulkanRenderer>(*window,
                                                         meshRegistry_->cache(),
                                                         texRegistry_->cache(),
                                                         matRegistry_->cache(),
                                                         modelRegistry_,
                                                         viewportRegistry_,
                                                         *vulkCtx_,
                                                         ctx.services);

            renderer_->initializeUI();

            // register services, but make sure to UN-register them on unload
            ctx.services.provide<AssetCache<Mesh>>(meshRegistry_->cache());
            ctx.services.provide<IRenderer>(renderer_);
            ctx.services.provide<IMeshRegistry>(meshRegistry_);
            ctx.services.provide<ITextureRegistry>(texRegistry_);
            ctx.services.provide<TextureFactory>(texFactory_);
            ctx.services.provide<IMaterialRegistry>(matRegistry_);
            ctx.services.provide<IModelRegistry>(modelRegistry_);
            ctx.services.provide<IViewportRegistry>(viewportRegistry_);

            auto& panelReg = ctx.services.resolve<ui::PanelRegistry>();
            auto& actionManager = ctx.services.resolve<ui::ActionManager>();

            //Stats
            panelReg.registerPanel([&renderer = renderer_]() {
                return ui::createVulkanStatsPanel(renderer->getStats());
            });
            //actionManager.registerPanel("window.plugins.vulkan_stats", panelInfo.id, true);

            //Debug tools
            const ui::Panel& debugPanel = panelReg.registerPanel(
                [&renderer = renderer_]() { return ui::createRenderDebugPanel(*renderer); });
            actionManager.registerPanel(
                "window.plugins.vulkan_debug", debugPanel.displayName(), true);

            CHAI_LOG_INFO("Renderer service provided");

            return true;
        }

        void onUnload(PluginContext& ctx) override
        {
            if (renderer_)
                renderer_->waitIdle(); // probably unnecessary here

            auto& panelReg = ctx.services.resolve<ui::PanelRegistry>();
            panelReg.unregisterPanel("Vulkan Stats");

            viewportRegistry_->shutdown();
            renderer_->shutdownUI();

            // remove services
            ctx.services.remove<IViewportRegistry>();
            ctx.services.remove<TextureFactory>();
            ctx.services.remove<ITextureRegistry>();
            ctx.services.remove<IMeshRegistry>();
            ctx.services.remove<IRenderer>();
            ctx.services.remove<AssetCache<Mesh>>();
            ctx.services.remove<IMaterialRegistry>();
            ctx.services.remove<IModelRegistry>();

            // release all our resources and pointers
            meshRegistry_->cache()->releaseAll();
            texRegistry_->cache()->releaseAll();
            modelRegistry_->releaseAll();
            matRegistry_->cache()->releaseAll();

            resources_->graveyard().flushAll();

            viewportRegistry_.reset();
            renderer_.reset();

            meshRegistry_.reset();
            texRegistry_.reset();

            texFactory_.reset();
            resources_.reset();
            vulkCtx_.reset();

            CHAI_LOG_INFO("Renderer removed");
        }

    private:
        std::shared_ptr<VulkanContext> vulkCtx_;
        std::shared_ptr<GpuResources> resources_;
        std::shared_ptr<VulkanRenderer> renderer_;

        // Mesh
        std::shared_ptr<MeshRegistry> meshRegistry_;

        // textures
        std::shared_ptr<TextureFactory> texFactory_;
        std::shared_ptr<TextureRegistry> texRegistry_;

        // Models
        std::shared_ptr<ModelRegistry> modelRegistry_;

        // Materials
        std::shared_ptr<MaterialRegistry> matRegistry_;

        std::shared_ptr<ViewportRegistry> viewportRegistry_;
    };

    CHAI_PLUGIN(VulkanPlugin);
} // namespace chai::gfx