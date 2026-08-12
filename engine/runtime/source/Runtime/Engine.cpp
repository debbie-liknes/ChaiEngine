#include <Runtime/Engine.h>
#include <Log.h>
#include <Window/Window.h>
#include <Scene/Scene.h>
#include <Input/IInput.h>
#include <UI/Editor/ActionManager.h>
#include <UI/Editor/InternalChaiUI.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/PanelHost.h>
#include <UI/Editor/DockspaceService.h>
#include <Runtime/SystemPaths.h>
#include <Audio/IAudioEngine.h>
#include <LogPanel.h>
#include <Visitors/AudioSceneVisitor.h>
#include <Visitors/FrameRenderVisitor.h>
#include <Graph/Algorithms.h>
#include <tracy/Tracy.hpp>

#include <unordered_set>

namespace chai
{
    void buildAdjacencyList(std::vector<IPlugin*>& plugins,
        std::unordered_map<uint32_t, std::vector<uint32_t>>& adjList,
        std::vector<uint32_t>& inDegree,
        const std::unordered_set<std::type_index>& providedServicesByEngine)
    {
        adjList.clear();
        inDegree.assign(plugins.size(), 0);

        std::unordered_map<std::type_index, uint32_t> service2Plugin;

        for (uint32_t i = 0; i < plugins.size(); i++)
            for (const auto& service : plugins[i]->providedServices())
                service2Plugin.insert_or_assign(service, i);

        for (uint32_t i = 0; i < plugins.size(); i++) {
            std::unordered_set<uint32_t> pluginsRequired;
            for (const auto& service : plugins[i]->requiredServices()) {
                if (auto itr = service2Plugin.find(service); itr != service2Plugin.end()) {
                    pluginsRequired.insert(service2Plugin[service]);
                    inDegree[service2Plugin[service]]++;
                }
                else if (!providedServicesByEngine.contains(service)) {
                    CHAI_LOG_CRITICAL("Unresolved service dependency for plugin {}",
                                      plugins[i]->name());
                }
            }
            adjList.insert_or_assign(
                i, std::vector<uint32_t>{pluginsRequired.begin(), pluginsRequired.end()});
        }

    }

    Engine::~Engine()
    {
    }

    void Engine::startup()
    {
        // Register panel management and docking services
        auto panelRegistry = std::make_shared<ui::PanelRegistry>();
        ctx_.services.provide<ui::PanelRegistry>(panelRegistry);

        auto configFile = executableDir() / "assets/editor/config/action_config.json";
        auto actionManager = std::make_shared<ui::ActionManager>(configFile, panelRegistry.get());
        ctx_.services.provide<ui::ActionManager>(actionManager);

        auto panelHost = std::make_shared<ui::PanelHost>();
        ctx_.services.provide<ui::PanelHost>(panelHost);

        auto dockspace = std::make_shared<ui::DockspaceService>();
        ctx_.services.provide<ui::DockspaceService>(dockspace);

        //Load plugins
        CHAI_LOG_INFO("Engine starting");

        std::unordered_set<std::type_index> providedServicesByEngine{typeid(ui::PanelRegistry),
            typeid(ui::ActionManager), typeid(ui::PanelHost), typeid(ui::DockspaceService)};

        std::unordered_map<uint32_t, std::vector<uint32_t>> adjList;
        std::vector<uint32_t> inDegree;
        buildAdjacencyList(plugins_, adjList, inDegree, providedServicesByEngine);

        std::vector<uint32_t> sortedPlugins;
        if (!graph::topologicalSort(plugins_, sortedPlugins, adjList, inDegree)) {
            CHAI_LOG_ERROR("Engine: Plugin dependency cycle detected. Some plugin depends on itself indirectly.");
        }

        for (const auto& i : sortedPlugins | std::views::reverse) {
            plugins_[i]->onLoad(ctx_);
            active_.push_back(plugins_[i]);
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
        ctx_.services.remove<ui::ActionManager>();
        ctx_.services.remove<ui::EditorViewportManager>();
        ctx_.services.remove<ui::PanelHost>();
        ctx_.services.remove<ui::DockspaceService>();
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
        auto& actionManager = services_.resolve<ui::ActionManager>();

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
            CHAI_LOG_WARN("Could not locate Audio Service. Audio capability will be disabled.");
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
            panelHost.draw(panelRegistry, dockingService, actionManager);

            actionManager.update(*input);

            scene_->accept(&audioVisitor);
            scene_->accept(&frameVisitor);
            
            if (audio) {
                audio->set3dListenersAndOrientations(audioVisitor.getData());
                audio->update();
            }

            renderer->renderFrame(frameVisitor.getData());
            renderer->endFrame();

            audioVisitor.reset();
            frameVisitor.reset();

            FrameMarkEnd("Engine");
        }
    }
}
