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
    if (!meshes || !textures) {
        CHAI_LOG_CRITICAL("Required registries missing.");
        return 1;
    }

    //build the scene
    auto scene = std::make_unique<Scene>();

    Handle<gfx::Mesh> cube = meshes->ingest(makeAssetId("builtin:cube"), gfx::makeCube(1.0f));
    Handle<gfx::Texture> crate =
        textures->load(makeAssetId("tex:tardis"), assetDir() / "tardis.png");

    GameObject* cubeA = scene->createObject("cubeA");
    auto* meshA = cubeA->addComponent<MeshComponent>();
    meshA->setMesh(cube);
    meshA->setTexture(crate);
    cubeA->getComponent<TransformComponent>()->setPosition({1, 0, 0});

    GameObject* cubeB = scene->createObject("cubeB");
    auto* meshB = cubeB->addComponent<MeshComponent>();
    meshB->setMesh(cube); 
    meshB->setMaterial(1);
    cubeB->getComponent<TransformComponent>()->setPosition({-1, 0, 0});
    auto* controlB = cubeB->addComponent<ControllerComponent>();
    controlB->addController<SpinController>();

    GameObject* cam = scene->createObject("camera");
    auto* camComp = cam->addComponent<CameraComponent>();
    camComp->setFOV(math::radians(60.f));
    camComp->setNearPlane(0.1f);
    camComp->setFarPlane(100.f);
    cam->getComponent<TransformComponent>()->setPosition({0, 0, 3});
    scene->setCamera(cam);

    GameObject* sun = scene->createObject("sun");
    sun->addComponent<LightComponent>();
    scene->setLight(sun);

    engine.setScene(std::move(scene));
    engine.run();

    engine.shutdown();
}