#include <Runtime/Editor.h>

#include <Audio/IAudioEngine.h>
#include <Assets/IMaterialRegistry.h>
#include <Assets/IMeshRegistry.h>
#include <Assets/IModelRegistry.h>
#include <Assets/ITextureRegistry.h>
#include <Assets/MaterialAsset.h>
#include <Assets/MeshAsset.h>
#include <Assets/PrimitiveMeshes.h>
#include <Components/CameraComponent.h>
#include <Components/ControllerComponent.h>
#include <Components/LightComponent.h>
#include <Components/MeshComponent.h>
#include <Components/SkyboxComponent.h>
#include <Components/TransformComponent.h>
#include <Controllers/FlyCamController.h>
#include <Controllers/SpinController.h>
#include <Runtime/Engine.h>
#include <Runtime/SystemPaths.h>
#include <Loaders/ITextureLoader.h>
#include <Loaders/TomlSettingsLoader.h>
#include <Log.h>
#include <Plugin/PluginLoader.h>
#include <Registry/SettingsRegistry.h>
#include <Plugin/PluginManager.h>
#include <Rendering/IRenderer.h>
#include <Scene/GameObject.h>
#include <Scene/Scene.h>
#include <Scene/SpawnPrefab.h>
#include <SpdLogSink.h>
#include <Window/Window.h>
#include <DiagnosticsUI/LogPanel.h>
#include <EditorUI/ActionManager.h>
#include <EditorUI/CommandPalette.h>
#include <EditorUI/PanelHost.h>
#include <EditorUI/PanelRegistry.h>
#include <EditorUI/EditorViewportManager.h>
#include <EditorUI/DockspaceService.h>
#include <EditorUI/SceneHierarchy.h>
#include <UI/SettingsPanel.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

namespace chai
{
    void setupDockspace(const chai::ServiceLocator& locator, chai::scene::Scene& scene)
    {
        using namespace chai;
        using namespace ui;
        using namespace scene;


        auto& panelRegistry = locator.resolve<PanelRegistry>();
        auto& vpManager = locator.resolve<EditorViewportManager>();
        auto& dockspace = locator.resolve<DockspaceService>();

        std::string mainPanelId = vpManager.addViewport("Main Scene", scene.getCameraId());

        std::string hierarchy = "Hierarchy";
        panelRegistry.registerPanel({.id = hierarchy, .displayName = hierarchy, .draw = [&scene] {
                                        ui::drawSceneHierarchy(scene);
                                }});

        ui::DockSplit horizontalSplit;
        horizontalSplit.ratio = 0.25f;
        horizontalSplit.side = ui::DockSplit::Side::Left;
        horizontalSplit.windowId = hierarchy;

        ui::DockSplit split;
        split.side = ui::DockSplit::Side::Bottom;
        split.ratio = 0.25f;
        split.windowId = "Logger";

        dockspace.setDefaultLayout({horizontalSplit, split}, mainPanelId);
    }

    void Editor::startup()
    {
    }

    void Editor::shutdown()
    {
        engine_->shutdown();

        engine_->services().remove<ui::PanelRegistry>();
        engine_->services().remove<ui::ActionManager>();
        engine_->services().remove<ui::EditorViewportManager>();
        engine_->services().remove<ui::PanelHost>();
        engine_->services().remove<ui::DockspaceService>();
    }

    void Editor::requestStop()
    {
        engine_->requestStop();
    }

    void Editor::run()
    {
        // these dont come from a plugin, guaranteed
        //auto& panelRegistry = engine_->services().resolve<ui::PanelRegistry>();
        //auto& panelHost = engine_->services().resolve<ui::PanelHost>();
        //auto& dockingService = engine_->services().resolve<ui::DockspaceService>();
        //auto& actionManager = engine_->services().resolve<ui::ActionManager>();
        using namespace chai;
        using namespace scene;

        // setup logging
        addLogSink(logSink_.get());
        setLogLevel(chai::LogLevel::Info);

        addLogSink(guiSink_.get());

        // Register panel management and docking services
        auto panelRegistry = std::make_shared<ui::PanelRegistry>();
        engine_->services().provide<ui::PanelRegistry>(panelRegistry);

        auto configFile = executableDir() / "assets/editor/config/action_config.json";
        auto actionManager = std::make_shared<ui::ActionManager>(configFile, panelRegistry.get());
        engine_->services().provide<ui::ActionManager>(actionManager);

        auto panelHost = std::make_shared<ui::PanelHost>();
        engine_->services().provide<ui::PanelHost>(panelHost);

        auto dockspace = std::make_shared<ui::DockspaceService>();
        engine_->services().provide<ui::DockspaceService>(dockspace);

        auto exeDir = executableDir();
        if (exeDir.empty())
            exeDir = std::filesystem::current_path();
        loader_->loadDirectory(exeDir / "plugins");
        engine_->setPlugins(loader_->plugins());

        engine_->startup();

        auto registry = engine_->services().tryResolve<gfx::IViewportRegistry>();
        if (!registry) {
            CHAI_LOG_CRITICAL("Could not locate Viewport Registry.");
        }

        auto vpManager = std::make_shared<ui::EditorViewportManager>(*registry, *panelRegistry);
        engine_->services().provide<ui::EditorViewportManager>(vpManager);

        engine_->services().provide<settings::SettingsRegistry>(
            std::make_shared<settings::SettingsRegistry>());

        actionManager->registerAction("file.exit", []() { exit(0); });

        ui::PanelDesc pluginPanel;
        pluginPanel.displayName = "Plugin Manager";
        pluginPanel.id = "PluginManager";
        pluginPanel.draw = [&]() { drawPluginManager(*loader_); };
        pluginPanel.visible = false;
        panelRegistry->registerPanel(pluginPanel);
        actionManager->registerPanel("window.plugin_manager", pluginPanel.id);

        ui::PanelDesc loggerPanel;
        loggerPanel.displayName = "Logger";
        loggerPanel.id = "logger";
        loggerPanel.draw = [&]() { diagnostics::drawLogPanel(*guiSink_); };
        loggerPanel.visible = true;
        panelRegistry->registerPanel(loggerPanel);
        actionManager->registerPanel("window.logger", loggerPanel.id);

        ui::PanelDesc commandPalette;
        commandPalette.displayName = "Command Palette";
        commandPalette.id = "CommandPalette";
        commandPalette.draw = [&]() { ui::drawCommandPalette(*actionManager); };
        commandPalette.visible = false;
        panelRegistry->registerPanel(commandPalette);
        actionManager->registerPanel("window.command_palette", commandPalette.id);

        ui::PanelDesc settingsPanel;
        settingsPanel.displayName = "Settings";
        settingsPanel.id = "settings";
        settingsPanel.draw = [&]() { settings::drawSettingsPanel(); };
        settingsPanel.visible = false;
        panelRegistry->registerPanel(settingsPanel);
        actionManager->registerPanel("file.preferences.settings", settingsPanel.id);

        auto meshes = engine_->services().tryResolve<gfx::IMeshRegistry>();
        auto textures = engine_->services().tryResolve<gfx::ITextureRegistry>();
        auto models = engine_->services().tryResolve<gfx::IModelRegistry>();
        auto materials = engine_->services().tryResolve<gfx::IMaterialRegistry>();
        auto audio = engine_->services().tryResolve<audio::IAudioEngine>();
        auto settings = engine_->services().tryResolve<settings::SettingsRegistry>();
        if (!meshes || !textures || !models || !settings) {
            CHAI_LOG_CRITICAL("Required registries missing.");
            return;
        }
        textures->load(makeAssetId("texture:9slice"), assetDir() / "9slice.png");

        auto input = engine_->services().tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not get input services.");
            return;
        }

        // build the scene
        auto& scene = engine_->scene();

        // auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
        // "SponzaHiRes/NewSponza_Main_glTF_003.glTF");
        auto prefab =
            models->load(makeAssetId("model:sponza"), assetDir() / "Sponza/glTF/Sponza.gltf");
        // auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
        // "ABeautifulGame/glTF/ABeautifulGame.gltf");

        // audio->playSound((assetDir() / "orchestral_techno.wav").string(), {0, 0, 0}, -10);

        if (prefab) {
            auto prefabInstance = scene::spawn(scene, *prefab);
        }

        GameObject* cam = scene.createObject("camera");
        auto* camComp = cam->addComponent<CameraComponent>();
        camComp->setFOV(math::radians(60.f));
        camComp->setNearPlane(0.1f);
        camComp->setFarPlane(100.f);
        cam->getComponent<TransformComponent>()->setPosition({0, 0, 3});
        auto* controlCam = cam->addComponent<ControllerComponent>();
        controlCam->addController<FlyCameraController>();
        scene.setCamera(cam);

        GameObject* sun = scene.createObject("sun");
        sun->addComponent<LightComponent>();
        auto sunLoc = sun->getComponent<TransformComponent>();
        sunLoc->lookAt(math::Vec3{-0.5, -1, -0.4f}, math::Vec3{0, 1, 0});
        scene.setLight(sun);

        GameObject* sky = scene.createObject("skybox");
        auto skyboxComp = sky->addComponent<SkyboxComponent>();
        std::array<std::filesystem::path, 6> skyTextures{assetDir() / "skybox/cubemap_0.png",
                                                         assetDir() / "skybox/cubemap_1.png",
                                                         assetDir() / "skybox/cubemap_2.png",
                                                         assetDir() / "skybox/cubemap_3.png",
                                                         assetDir() / "skybox/cubemap_4.png",
                                                         assetDir() / "skybox/cubemap_5.png"};
        auto skyBoxTex = textures->loadCubemap(makeAssetId("component:skybox"), skyTextures);
        skyboxComp->setTexture(skyBoxTex);

        setupDockspace(engine_->services(), scene);

        engine_->run([&panelRegistry, &dockspace, &panelHost, &actionManager](
                         const UpdateContext& ctx) {
            //draw internal uis
            panelHost->draw(*panelRegistry, *dockspace, *actionManager);
            actionManager->update(ctx.input);
        });
    }

} // namespace chai
