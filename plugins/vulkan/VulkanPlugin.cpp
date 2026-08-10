/**
 * @file VulkanPlugin.h
 * @brief Plugin setup and registration of services
 */
#include "VulkanPluginStatsPanel.h"
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
#include <Log.h>
#include <Plugin/PluginBase.h>
#include <Plugin/PluginMacros.h>
#include <Plugin/ServiceLocator.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/MenuService.h>
#include <Window/Window.h>
#include <memory>

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
            matRegistry_ = std::make_shared<MaterialRegistry>(
                *vulkCtx_, texRegistry_->cache(), resources_->graveyard());
            modelRegistry_ = std::make_shared<ModelRegistry>(
                *meshRegistry_, *texRegistry_, *matRegistry_, &ctx.services);
            renderer_ = std::make_shared<VulkanRenderer>(*window,
                                                         meshRegistry_->cache(),
                                                         texRegistry_->cache(),
                                                         matRegistry_->cache(),
                                                         modelRegistry_,
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

            ui::PanelDesc panelInfo;
            panelInfo.displayName = "Vulkan Stats";
            panelInfo.draw = [&]() { ui::drawVulkanStatsPanel(renderer_->getStats()); };
            panelInfo.visible = false;

            auto& panelReg = ctx.services.resolve<ui::PanelRegistry>();
            panelReg.registerPanel(panelInfo);
            auto& menuService = ctx.services.resolve<ui::MenuService>();
            //menuService.registerAction("Windows/Plugins/Vulkan", ui::TogglePanel{panelInfo.displayName});

            CHAI_LOG_INFO("Renderer service provided");
        }

        void onUnload(PluginContext& ctx) override
        {
            if (renderer_)
                renderer_->waitIdle(); // probably unnecessary here

            auto panelReg = ctx.services.resolve<ui::PanelRegistry>();
            panelReg.unregisterPanel("Vulkan Stats");
            renderer_->shutdownUI();

            // remove services
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
    };

    CHAI_PLUGIN(VulkanPlugin);
} // namespace chai::gfx