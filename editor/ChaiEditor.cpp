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
#include <Core/Engine.h>
#include <Core/SystemPaths.h>
#include <Loaders/ITextureLoader.h>
#include <Log.h>
#include <Plugin/PluginLoader.h>
#include <Rendering/IRenderer.h>
#include <Scene/GameObject.h>
#include <Scene/Scene.h>
#include <Scene/SpawnPrefab.h>
#include <SpdLogSink.h>
#include <Window/Window.h>
#include <LogPanel.h>
#include <UI/Editor/MenuService.h>
#include <UI/Editor/PanelRegistry.h>
#include <UI/Editor/EditorViewportManager.h>
#include <UI/Editor/DockspaceService.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

void setupDockspace(chai::ServiceLocator& locator, chai::scene::Scene& scene)
{
    using namespace chai;
    using namespace ui;
    using namespace scene;


    auto& panelRegistry = locator.resolve<PanelRegistry>();
    auto& vpManager = locator.resolve<EditorViewportManager>();
    auto& dockspace = locator.resolve<DockspaceService>();

    std::string mainPanelId = vpManager.addViewport("Main Scene", scene.getCameraId());

    auto sceneIds = scene.registerPanels(locator);

    ui::DockSplit horizontalSplit;
    horizontalSplit.ratio = 0.25f;
    horizontalSplit.side = ui::DockSplit::Side::Left;
    horizontalSplit.windowId = sceneIds.hierarchy;

    ui::DockSplit split;
    split.side = ui::DockSplit::Side::Bottom;
    split.ratio = 0.25f;
    split.windowId = "Logger";

    dockspace.setDefaultLayout({horizontalSplit, split}, mainPanelId);
}

int main()
{
    using namespace chai;
    using namespace scene;

    // setup logging
    SpdLogSink logSink;
    addLogSink(&logSink);
    setLogLevel(chai::LogLevel::Info);

    diagnostics::GuiLogSink guiSink;
    addLogSink(&guiSink);

    Engine engine;
    PluginLoader loader;
    auto exeDir = executableDir();
    if (exeDir.empty())
        exeDir = std::filesystem::current_path();
    loader.loadDirectory(exeDir / "plugins");
    engine.setPlugins(loader.plugins());
    engine.startup();

    ui::PanelDesc panelInfo;
    panelInfo.displayName = "Logger";
    panelInfo.id = "Logger";
    panelInfo.draw = [&]() { diagnostics::drawLogPanel(guiSink); };
    panelInfo.visible = true;
    auto& panelReg = engine.services().resolve<ui::PanelRegistry>();
    panelReg.registerPanel(panelInfo);
    auto& menuService = engine.services().resolve<ui::MenuService>();
    menuService.registerItem("Windows/Logger", ui::TogglePanel{panelInfo.id});

    auto meshes = engine.services().tryResolve<gfx::IMeshRegistry>();
    auto textures = engine.services().tryResolve<gfx::ITextureRegistry>();
    auto models = engine.services().tryResolve<gfx::IModelRegistry>();
    auto materials = engine.services().tryResolve<gfx::IMaterialRegistry>();
    if (!meshes || !textures || !models) {
        CHAI_LOG_CRITICAL("Required registries missing.");
        return 1;
    }

    auto input = engine.services().tryResolve<IInput>();
    if (!input) {
        CHAI_LOG_CRITICAL("Could not get input services.");
        return 1;
    }

    // build the scene
    auto scene = std::make_unique<Scene>();


    //auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
    //"Sponza/intel/main_sponza/NewSponza_Main_glTF_003.glTF");
    auto prefab = models->load(makeAssetId("model:sponza"), assetDir() / "Sponza/glTF/Sponza.gltf");
    //auto prefab = models->load(makeAssetId("model:sponza"), assetDir() / "ABeautifulGame/glTF/ABeautifulGame.gltf");

    if (prefab) {
        auto prefabInstance = scene::spawn(*scene, *prefab);
    }

    GameObject* cam = scene->createObject("camera");
    auto* camComp = cam->addComponent<CameraComponent>();
    camComp->setFOV(math::radians(60.f));
    camComp->setNearPlane(0.1f);
    camComp->setFarPlane(100.f);
    cam->getComponent<TransformComponent>()->setPosition({0, 0, 3});
    auto* controlCam = cam->addComponent<ControllerComponent>();
    controlCam->addController<FlyCameraController>();
    scene->setCamera(cam);

    GameObject* sun = scene->createObject("sun");
    sun->addComponent<LightComponent>();
    auto sunLoc = sun->getComponent<TransformComponent>();
    sunLoc->lookAt(math::Vec3{-0.5, -1, -0.4f}, math::Vec3{0, 1, 0});
    scene->setLight(sun);

    GameObject* sky = scene->createObject("skybox");
    auto skyboxComp = sky->addComponent<SkyboxComponent>();
    std::array<std::filesystem::path, 6> skyTextures{assetDir() / "skybox/cubemap_0.png",
                                                     assetDir() / "skybox/cubemap_1.png",
                                                     assetDir() / "skybox/cubemap_2.png",
                                                     assetDir() / "skybox/cubemap_3.png",
                                                     assetDir() / "skybox/cubemap_4.png",
                                                     assetDir() / "skybox/cubemap_5.png"};
    auto skyBoxTex = textures->loadCubemap(makeAssetId("component:skybox"), skyTextures);
    skyboxComp->setTexture(skyBoxTex);

    setupDockspace(engine.services(), *scene);
    engine.setScene(std::move(scene));
    engine.run();

    engine.shutdown();
}
