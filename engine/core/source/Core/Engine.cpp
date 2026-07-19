#include <Core/Engine.h>
#include <Log.h>
#include <Window/Window.h>
#include <Rendering/FrameRenderData.h>
#include <Core/IInput.h>
#include <UI/Tools/InternalChaiUi.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/PanelHost.h>
#include <UI/Editor/DockspaceService.h>
#include <Core/SystemPaths.h>
#include <LogPanel.h>

namespace chai
{
    Engine::~Engine()
    {
    }

    void Engine::startup()
    {
        // Register panel management and docking services
        auto panelRegistry = std::make_shared<ui::PanelRegistry>();
        ctx_.services.provide<ui::PanelRegistry>(panelRegistry);

        auto panelHost = std::make_shared<ui::PanelHost>();
        ctx_.services.provide<ui::PanelHost>(panelHost);

        auto dockspace = std::make_shared<ui::DockspaceService>();
        ctx_.services.provide<ui::DockspaceService>(dockspace);

        //Load plugins
        CHAI_LOG_INFO("Engine starting");
        for (auto& p : plugins_) {
            p->onLoad(ctx_);
            active_.push_back(p);
        }

        auto renderer = services_.tryResolve<gfx::IRenderer>();
        if (!renderer) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto vpManager = std::make_shared<ui::EditorViewportManager>(*renderer, *panelRegistry);
        ctx_.services.provide<ui::EditorViewportManager>(vpManager);

        std::string mainPanelId = vpManager->addPane(*renderer, "Main Scene", 0);

        std::string hierarchy = "Hierarchy";
        panelRegistry->registerPanel({.id = hierarchy, .displayName = "Hierarchy", .draw = [] {
                                          ui::Text("Scene hierarchy tree goes here");
                                      }});

        ui::DockSplit horizontalSplit;
        horizontalSplit.ratio = 0.25f;
        horizontalSplit.side = ui::DockSplit::Side::Left;
        horizontalSplit.windowId = hierarchy;

        ui::DockSplit split;
        split.side = ui::DockSplit::Side::Bottom;
        split.ratio = 0.25f;
        split.windowId = "Logger";

        dockspace->setDefaultLayout({horizontalSplit, split}, mainPanelId);


        ui::loadFonts(executableDir().string() + "/assets/editor/fonts");
    }

    void Engine::shutdown()
    {
        CHAI_LOG_INFO("Engine shutdown");
        for (auto it = active_.rbegin(); it != active_.rend(); ++it)
            (*it)->onUnload(ctx_);

        ctx_.services.remove<ui::PanelRegistry>();
        ctx_.services.remove<ui::EditorViewportManager>();
    }

    void Engine::requestStop()
    {
        running_ = false;
    }

    void Engine::setPlugins(std::span<IPlugin* const> p)
    {
        plugins_.assign(p.begin(), p.end());
    }

    void Engine::setScene(std::unique_ptr<IScene> scene)
    {
        scene_ = std::move(scene);
    }

    void Engine::run()
    {
        //these dont come from a plugin, guaranteed
        auto& panelRegistry = services_.resolve<ui::PanelRegistry>();
        auto& panelHost = services_.resolve<ui::PanelHost>();
        auto& dockingService = services_.resolve<ui::DockspaceService>();

        //These come from plugins, check that they exist
        auto window = services_.tryResolve<IWindow>();
        if (!window) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto renderer = services_.tryResolve<gfx::IRenderer>();
        if (!renderer) {
            CHAI_LOG_CRITICAL("Could not locate Renderer Service.");
        }

        auto input = services_.tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not locate Input Service.");
        }

        // The main guts of the application
        while (!window->shouldClose()) {
            input->newFrame();  //tell input to clear deltas FIRST
            window->pollEvents();
            renderer->startFrame();
            float dt = clock_.tick();

            updateActiveCameraAspect();
            UpdateContext ctx{dt, *input};
            scene_->update(ctx);

            //draw internal uis
            panelHost.draw(panelRegistry, dockingService);

            gfx::FrameRenderData frame;
            scene_->extract(frame);
            renderer->renderFrame(frame);
            renderer->endFrame();
        }
    }

    void Engine::updateActiveCameraAspect()
    {
        auto window = services().tryResolve<IWindow>();
        if (!window || !scene_)
            return; // headless?

        //get framebuffer size
        int w = 0, h = 0;
        window->framebufferSize(w, h);
        if (w == 0 || h == 0)
            return; // minimized

        float aspect = float(w) / float(h);
        scene_->setCameraAspect(aspect);
    }
}