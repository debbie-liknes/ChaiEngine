#include <Engine.h>
#include <SpdLogSink.h>
#include <Log.h>
#include <Window/Window.h>
#include <Plugin/PluginLoader.h>
#include <SystemPaths.h>
#include <Rendering/IRenderer.h>
#include <Assets/PrimitiveMeshes.h>
#include <Assets/MeshAsset.h>
#include <Assets/IMeshRegistry.h>
#include <Scene/GameObject.h>
#include <Components/MeshComponent.h>
#include <Components/CameraComponent.h>
#include <Components/TransformComponent.h>
#include <Components/LightComponent.h>
#include <Loaders/ITextureLoader.h>
#include <Assets/ITextureRegistry.h>
#include <Scene/Scene.h>
#include <Components/ControllerComponent.h>
#include <Controllers/SpinController.h>
#include <Controllers/FlyCamController.h>
#include <Assets/IModelRegistry.h>
#include <Assets/MaterialAsset.h>
#include <Assets/IMaterialRegistry.h>
#include <Scene/SpawnPrefab.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

int main()
{
    using namespace chai;
    using namespace scene;

    //setup logging
    SpdlogSink logSink; 
    setLogSink(&logSink);
    setLogLevel(chai::LogLevel::Info);

    Engine engine;
    PluginLoader loader;
    auto exeDir = executableDir();
    if (exeDir.empty())
        exeDir = std::filesystem::current_path();
    loader.loadDirectory(exeDir / "plugins");
    engine.setPlugins(loader.plugins());
    engine.startup();

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

    //build the scene
    auto scene = std::make_unique<Scene>();

    auto prefab = models->load(makeAssetId("model:sponza"), assetDir() / "Sponza/intel/main_sponza/NewSponza_Main_glTF_003.glTF");
    //auto prefab = models->load(makeAssetId("model:demo"), assetDir() / "VirtualCity/glTF/VirtualCity.glTF");

    if (prefab)
    {
        auto prefabInstance = scene::spawn(*scene, *prefab);
        //prefabInstance.root->getComponent<TransformComponent>()->setScale(math::Vec3{5.f, 5.f, 5.f});
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
    sun->getComponent<TransformComponent>()->lookAt(math::Vec3{-0.5, -1, 0}, math::Vec3{0, 1, 0});
    scene->setLight(sun);

    engine.setScene(std::move(scene));
    engine.run();

    engine.shutdown();
}