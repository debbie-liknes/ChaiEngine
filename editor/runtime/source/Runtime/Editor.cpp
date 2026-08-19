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
#include <OS/SystemPaths.h>
#include <Loaders/ITextureLoader.h>
#include <Loaders/TomlSettingsLoader.h>
#include <Log.h>
#include <EditorUI/PluginManager.h>
#include <Rendering/IRenderer.h>
#include <Scene/GameObject.h>
#include <Scene/Scene.h>
#include <Scene/ObjectTable.h>
#include <Scene/Object.h>
#include <Scene/SpawnPrefab.h>
#include <Window/Window.h>
#include <EditorUI/CommandPalette.h>
#include <UI/Core/InternalChaiUI.h>
#include <UI/SettingsPanel.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

namespace chai
{
    void Editor::setupDockspace(const ServiceLocator& locator, scene::Scene& scene)
    {
        using namespace ui;
        using namespace scene;

        auto& panelRegistry = locator.resolve<PanelRegistry>();
        auto& vpManager = locator.resolve<EditorViewportManager>();
        auto& dockspace = locator.resolve<DockspaceService>();

        std::string mainPanelId = vpManager.addViewport("Main Scene", scene.getCameraId());

        ui::PanelDesc propertiesPanel;
        propertiesPanel.displayName = "Properties";
        propertiesPanel.id = "Properties";
        propertiesPanel.draw = [&]() {
            const auto selectedItem = editorSelection_.getSelected();
            // TODO: multi select
            Object* selected = ObjectTable::instance().resolve(selectedItem.id);
            ui::drawPropertiesPane(selected);
        };
        propertiesPanel.visible = false;
        panelRegistry_->registerPanel(propertiesPanel);
        actionManager_->registerPanel("window.properties", propertiesPanel.id, true);

        std::string hierarchy = "Hierarchy";
        panelRegistry.registerPanel({
            .id = hierarchy,
            .displayName = hierarchy,
            .draw = [&scene, &selector = editorSelection_, &panelRegistry = panelRegistry_] {
                 auto selectedCallback = [&](const scene::ObjectId id) {
                    selector.select({id});
                    const auto& panel = panelRegistry->getPanel("Properties");
                    if (!panel->visible) {
                        panel->visible = true;
                     }
                 };
                 ui::drawSceneHierarchy(scene, selectedCallback);
            }});

        ui::DockSplit hierarchySplit;
        hierarchySplit.ratio = 0.25f;
        hierarchySplit.side = ui::DockSplit::Side::Left;
        hierarchySplit.windowId = hierarchy;

        ui::DockSplit propertiesSplit;
        propertiesSplit.ratio = 0.25f;
        propertiesSplit.side = ui::DockSplit::Side::Right;
        propertiesSplit.windowId = "Properties";

        ui::DockSplit split;
        split.side = ui::DockSplit::Side::Bottom;
        split.ratio = 0.25f;
        split.windowId = "Logger";

        dockspace.setDefaultLayout({hierarchySplit, propertiesSplit, split}, mainPanelId);
    }

    void Editor::registerActions() const
    {
        //panelReg.onPanelRegistered([&actionManager = actionManager_](const ui::Panel& p) {
        //    actionManager.registerPanel(p);
        //});

        actionManager_->registerAction("file.exit", []() { exit(0); });

        ui::PanelDesc pluginPanel;
        pluginPanel.displayName = "Plugin Manager";
        pluginPanel.id = "Plugin Manager";
        pluginPanel.draw = [&]() { drawPluginManager(*loader_); };
        pluginPanel.visible = false;
        panelRegistry_->registerPanel(pluginPanel);
        actionManager_->registerPanel("window.plugin_manager", pluginPanel.id);

        ui::PanelDesc loggerPanel;
        loggerPanel.displayName = "Logger";
        loggerPanel.id = "Logger";
        loggerPanel.draw = [&]() { diagnostics::drawLogPanel(*guiSink_); };
        loggerPanel.visible = true;
        panelRegistry_->registerPanel(loggerPanel);
        actionManager_->registerPanel("window.logger", loggerPanel.id);

        ui::PanelDesc commandPalette;
        commandPalette.displayName = "Command Palette";
        commandPalette.id = "Command Palette";
        commandPalette.draw = [&]() { ui::drawCommandPalette(*actionManager_); };
        commandPalette.visible = false;
        panelRegistry_->registerPanel(commandPalette);
        actionManager_->registerPanel("window.command_palette", commandPalette.id);

        ui::PanelDesc settingsPanel;
        settingsPanel.displayName = "Settings";
        settingsPanel.id = "Settings";
        settingsPanel.draw = [&]() { settings::drawSettingsPanel(); };
        settingsPanel.visible = false;
        panelRegistry_->registerPanel(settingsPanel);
        actionManager_->registerPanel("file.preferences.settings", settingsPanel.id);
    }

    void setupDefaultScene(chai::scene::Scene& scene,
                           gfx::IModelRegistry& models,
                           gfx::ITextureRegistry& textures)
    {
        using namespace scene;

        // auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
        //"SponzaHiRes/NewSponza_Main_glTF_003.glTF");
        auto prefab = models.load(makeAssetId("model:sponza"), assetDir() / "Sponza/glTF/Sponza.gltf");
        //auto prefab = models.load(makeAssetId("model:sponza1"), assetDir() /
        //"Sponza/Intel/main_sponza/NewSponza_Main_glTF_003.glTF");

        // audio->playSound((assetDir() / "orchestral_techno.wav").string(), {0, 0, 0}, -10);

        if (prefab) {
            auto prefabInstance = scene::spawn(scene, *prefab, {"Sponza Root"});
        }

        //auto testObj = scene.createObject("Baby sponza");
        //testObj->getComponent<TransformComponent>()->setPosition(math::Vec3{50, 0, 0});
        //if (prefab1) {
        //    auto prefabInstance = scene::spawn(scene, *prefab1, {"Sponza Root1", testObj});
        //}

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
        auto skyBoxTex = textures.loadCubemap(makeAssetId("component:skybox"), skyTextures);
        skyboxComp->setTexture(skyBoxTex);
    }

    IPlugin::ServiceList Editor::providedServices() const
    {
        return {typeid(ui::PanelRegistry),
                typeid(ui::ActionManager),
                typeid(ui::PanelHost),
                typeid(ui::DockspaceService),
                typeid(ui::EditorViewportManager)};
    }

    bool Editor::startup()
    {
        using namespace scene;

        // setup logging
        addLogSink(logSink_.get());
        setLogLevel(chai::LogLevel::Info);

        addLogSink(guiSink_.get());

        // Register panel management and docking services
        panelRegistry_ = std::make_shared<ui::PanelRegistry>();
        engine_->services().provide<ui::PanelRegistry>(panelRegistry_);

        auto configFile = executableDir() / "assets/editor/config/action_config.json";
        actionManager_ = std::make_shared<ui::ActionManager>(configFile, panelRegistry_.get());
        engine_->services().provide<ui::ActionManager>(actionManager_);

        panelHost_ = std::make_shared<ui::PanelHost>();
        engine_->services().provide<ui::PanelHost>(panelHost_);

        dockspace_ = std::make_shared<ui::DockspaceService>();
        engine_->services().provide<ui::DockspaceService>(dockspace_);

        auto exeDir = executableDir();
        if (exeDir.empty())
            exeDir = std::filesystem::current_path();
        if (!loader_->loadDirectory(exeDir / "plugins")) {
            CHAI_LOG_CRITICAL("Editor: Failed to load plugins. Exiting prematurely.");
            return false;
        }

        engine_->setPlugins(loader_->plugins());
        if (!engine_->startup())
        {
            CHAI_LOG_CRITICAL("Engine failed to start. Exiting prematurely.");
            return false;
        }

        ui::loadFonts(executableDir().string() + "/assets/editor/fonts");

        auto viewportRegistry = engine_->services().tryResolve<gfx::IViewportRegistry>();
        if (!viewportRegistry) {
            CHAI_LOG_CRITICAL("Could not locate Viewport Registry.");
        }

        vpManager_ =
            std::make_shared<ui::EditorViewportManager>(*viewportRegistry, *panelRegistry_);
        engine_->services().provide<ui::EditorViewportManager>(vpManager_);

        settingsRegistry_ = std::make_shared<settings::SettingsRegistry>();
        engine_->services().provide<settings::SettingsRegistry>(settingsRegistry_);

        auto meshes = engine_->services().tryResolve<gfx::IMeshRegistry>();
        auto textures = engine_->services().tryResolve<gfx::ITextureRegistry>();
        auto models = engine_->services().tryResolve<gfx::IModelRegistry>();
        auto materials = engine_->services().tryResolve<gfx::IMaterialRegistry>();
        auto audio = engine_->services().tryResolve<audio::IAudioEngine>();
        auto settings = engine_->services().tryResolve<settings::SettingsRegistry>();
        if (!meshes || !textures || !models || !settings) {
            CHAI_LOG_CRITICAL("Required registries missing.");
            return false;
        }

        auto input = engine_->services().tryResolve<IInput>();
        if (!input) {
            CHAI_LOG_CRITICAL("Could not get input services.");
            return false;
        }

        registerActions();

        // build the scene
        auto& scene = engine_->scene();
        setupDefaultScene(scene, *models, *textures);

        setupDockspace(engine_->services(), scene);

        return true;
    }

    void Editor::shutdown()
    {
        engine_->shutdown();

        engine_->services().remove<ui::PanelRegistry>();
        engine_->services().remove<ui::ActionManager>();
        engine_->services().remove<ui::EditorViewportManager>();
        engine_->services().remove<ui::PanelHost>();
        engine_->services().remove<ui::DockspaceService>();
        engine_->services().remove<settings::SettingsRegistry>();
    }

    void Editor::requestStop()
    {
        engine_->requestStop();
    }

    void Editor::run()
    {
        engine_->run([this](
                         const UpdateContext& ctx) {
            //draw internal uis
            panelHost_->draw(*panelRegistry_, *dockspace_, *actionManager_);
            actionManager_->update(ctx.input);
        });
    }

} // namespace chai
