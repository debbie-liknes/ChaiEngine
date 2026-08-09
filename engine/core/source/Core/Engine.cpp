#include <Core/Engine.h>
#include <Log.h>
#include <Window/Window.h>
#include <Scene/Scene.h>
#include <Input/IInput.h>
#include <UI/Editor/InternalChaiUI.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/PanelHost.h>
#include <UI/Editor/DockspaceService.h>
#include <UI/Editor/MenuService.h>
#include <Core/SystemPaths.h>
#include <Audio/IAudioEngine.h>
#include <LogPanel.h>
#include <Visitors/AudioSceneVisitor.h>
#include <Visitors/FrameRenderVisitor.h>
#include <tracy/Tracy.hpp>

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

        auto menuService = std::make_shared<ui::MenuService>();
        ctx_.services.provide<ui::MenuService>(menuService);

        //Load plugins
        CHAI_LOG_INFO("Engine starting");
        for (auto& p : plugins_) {
            p->onLoad(ctx_);
            active_.push_back(p);
        }

        auto registry = services_.tryResolve<gfx::IViewportRegistry>();
        if (!registry) {
            CHAI_LOG_CRITICAL("Could not locate Viewport Registry.");
        }

        auto vpManager = std::make_shared<ui::EditorViewportManager>(*registry, *panelRegistry);
        ctx_.services.provide<ui::EditorViewportManager>(vpManager);

        ui::loadFonts(executableDir().string() + "/assets/editor/fonts");

        //Create scene
        scene_ = std::make_unique<scene::Scene>();
    }

    void Engine::shutdown()
    {
        CHAI_LOG_INFO("Engine shutdown");
        for (auto it = active_.rbegin(); it != active_.rend(); ++it)
            (*it)->onUnload(ctx_);

        ctx_.services.remove<ui::PanelRegistry>();
        ctx_.services.remove<ui::EditorViewportManager>();
        ctx_.services.remove<ui::PanelHost>();
        ctx_.services.remove<ui::DockspaceService>();
        ctx_.services.remove<ui::MenuService>();
    }

    void Engine::requestStop()
    {
        running_ = false;
    }

    void Engine::setPlugins(std::span<IPlugin* const> p)
    {
        plugins_.assign(p.begin(), p.end());
    }

    void Engine::run()
    {
        //these dont come from a plugin, guaranteed
        auto& panelRegistry = services_.resolve<ui::PanelRegistry>();
        auto& panelHost = services_.resolve<ui::PanelHost>();
        auto& dockingService = services_.resolve<ui::DockspaceService>();
        auto& menuService = services_.resolve<ui::MenuService>();

        //These come from plugins, check that they exist
        auto window = services_.tryResolve<IWindow>();
        if (!window) {
            CHAI_LOG_CRITICAL("Could not locate Window Service.");
        }

        auto renderer = services_.tryResolve<gfx::IRenderer>();
        if (!renderer) {
            CHAI_LOG_CRITICAL("Could not locate Renderer Service.");
        }

        auto audio = services_.tryResolve<audio::IAudioEngine>();
        if (!audio) {
            CHAI_LOG_CRITICAL("Could not locate Audio Service.");
        }

        auto input = services_.tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not locate Input Service.");
        }

        scene::AudioSceneVisitor audioVisitor;
        scene::FrameRenderVisitor frameVisitor;

        // The main guts of the application
        while (!window->shouldClose()) {
            FrameMarkStart("Engine");

            input->newFrame();  //tell input to clear deltas FIRST

            std::span<const WindowEvent> events{ window->pollEvents() };

            for (const WindowEvent& event : events) {

                // tell the renderer about resized events
                if (event.type == WindowEventType::Resized)
                    renderer->onResize(event.width, event.height);

            }


            renderer->startFrame();
            float dt = clock_.tick();

            UpdateContext ctx{dt, *input};
            scene_->update(ctx);

            //draw internal uis
            panelHost.draw(panelRegistry, dockingService, menuService);

            scene_->accept(&audioVisitor);
            scene_->accept(&frameVisitor);

            audio->set3dListenersAndOrientations(audioVisitor.getData());
            audio->update();

            renderer->renderFrame(frameVisitor.getData());
            renderer->endFrame();

            audioVisitor.reset();
            frameVisitor.reset();

            FrameMarkEnd("Engine");
        }
    }
}
