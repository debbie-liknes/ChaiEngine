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
#include <Loaders/ITextureLoader.h>
#include <Assets/ITextureRegistry.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

int main()
{
    using namespace chai;

    //setup logging
    SpdlogSink logSink; 
    setLogSink(&logSink);
    setLogLevel(chai::LogLevel::Info);

    chai::Engine engine;

    //Load plugins
    chai::PluginLoader loader;
    auto exeDir = chai::executableDir();
    if (exeDir.empty())
        exeDir = std::filesystem::current_path();
    loader.loadDirectory(exeDir / "plugins");
    engine.setPlugins(loader.plugins());

    engine.startup();

    //Try to make a window
    auto win = engine.services().tryResolve<IWindow>();
    if (!win)
    {
        CHAI_LOG_CRITICAL("Could not locate Window Service.");
        return 1;
    }

    auto renderer = engine.services().tryResolve<gfx::IRenderer>();
    if (!renderer)
    {
        CHAI_LOG_CRITICAL("Could not locate Renderer.");
        return 1;
    }

    static auto start = std::chrono::high_resolution_clock::now();

    auto registry = engine.services().tryResolve<gfx::IMeshRegistry>();

    //make an object
    auto cubeObj = std::make_shared<scene::GameObject>();
    auto meshComp = cubeObj->addComponent<scene::MeshComponent>();
    Handle<gfx::Mesh> cube = registry->ingest(makeAssetId("builtin:cube"), gfx::makeCube(1.0f));
    meshComp->setMesh(cube);
    auto cubeTrans = cubeObj->getComponent<scene::TransformComponent>();
    cubeTrans->setPosition(math::Vec3{1, 0, 0});
    
    auto cubeObj2 = std::make_shared<scene::GameObject>();
    auto meshComp2 = cubeObj2->addComponent<scene::MeshComponent>();
    Handle<gfx::Mesh> cube2 = registry->ingest(makeAssetId("builtin:cube2"), gfx::makeCube(1.0f));
    meshComp2->setMesh(cube);
    auto cubeTrans2 = cubeObj2->getComponent<scene::TransformComponent>();
    cubeTrans2->setPosition(math::Vec3{-1, 0, 0});


    auto cameraObj = std::make_shared<scene::GameObject>();
    auto camComp = cameraObj->addComponent<scene::CameraComponent>();

    auto camTrans = cameraObj->getComponent<scene::TransformComponent>();
    camTrans->setPosition(math::Vec3{0, 0, 3});

    auto textures = engine.services().tryResolve<gfx::ITextureRegistry>();
    if (!textures) {
        CHAI_LOG_ERROR("Could not find Texture Registry");
    }
    auto image = textures->load(makeAssetId("tex:crate"), assetDir() / "tardis.png");
    meshComp->setTexture(image);
    meshComp2->setTexture(image);
    meshComp2->setMaterial(1);

    //main loop
    while (!win->shouldClose()) {
        auto events = win->pollEvents();

        //temp
        auto now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(now - start).count(); // seconds since start
        float angle = time * math::radians(90.0f); // 90°/second, regardless of fps

        int w = 0, h = 0;
        win->framebufferSize(w, h);
        float aspect = 1.f * w / h;
        math::Quaternion q = math::Quat::fromAxisAngle(math::Vec3{0, 1, 0}, angle);
        //keep the cube spinning
        cubeObj->getComponent<scene::TransformComponent>()->setRotation(q);
        cubeObj2->getComponent<scene::TransformComponent>()->setRotation(q);

        //a scene should probably store these things, unsure how updating the camera would actually work. A controller?
        camComp->setAspectRatio(aspect);
        camComp->setFOV(math::radians(60.f));
        camComp->setNearPlane(0.1f);
        camComp->setFarPlane(100.f);


        cameraObj->update(time);
        cubeObj->update(time);
        cubeObj2->update(time);

        gfx::FrameRenderData frame;
        cubeObj->extract(frame);
        cubeObj2->extract(frame);
        camComp->extract(frame);   
        //end temp

        renderer->renderFrame(frame);
        engine.tick();
    }

    renderer.reset();
    win.reset();

    engine.shutdown();
}