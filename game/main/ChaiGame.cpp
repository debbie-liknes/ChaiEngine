#include <Runtime/Engine.h>
#include <OS/SystemPaths.h>
#include <Log.h>
#include <Plugin/PluginLoader.h>
#include <tracy/Tracy.hpp>
#include <Rendering/IRenderer.h>
#include <Assets/IModelRegistry.h>
#include <Assets/ITextureRegistry.h>
#include <Rendering/Viewport.h>
#include <Components/CameraComponent.h>
#include <Components/SkyboxComponent.h>
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Controllers/FlyCamController.h>
#include <Scene/SpawnPrefab.h>
#include <SpdLogSink.h>

std::filesystem::path assetDir()
{
    return CHAI_ASSET_DIR;
}

namespace chai
{
    void setupDefaultScene(chai::scene::Scene& scene,
                           gfx::IModelRegistry& models,
                           gfx::ITextureRegistry& textures)
    {
        using namespace scene;

        // auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
        //"SponzaHiRes/NewSponza_Main_glTF_003.glTF");
        auto prefab =
            models.load(makeAssetId("model:sponza"), assetDir() / "Sponza/glTF/Sponza.gltf");
        //  auto prefab = models->load(makeAssetId("model:sponza"), assetDir() /
        //  "ABeautifulGame/glTF/ABeautifulGame.gltf");

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
        auto skyBoxTex = textures.loadCubemap(makeAssetId("component:skybox"), skyTextures);
        skyboxComp->setTexture(skyBoxTex);
    }
} // namespace chai

int main()
{
    // Required to initialize tracy
    TracyNoop;

    using namespace chai;

	// setup logging
    std::unique_ptr<SpdLogSink> logSink_ = std::make_unique<SpdLogSink>();
    addLogSink(logSink_.get());
    setLogLevel(chai::LogLevel::Info);

    Engine engine;

	auto exeDir = executableDir();
	if (exeDir.empty())
		exeDir = std::filesystem::current_path();
    PluginLoader loader;
    if (!loader.loadDirectory(exeDir / "plugins")) {
        CHAI_LOG_CRITICAL("Editor: Failed to load plugins. Exiting prematurely.");
        return 1;
    }

    engine.setPlugins(loader.plugins());
    if (!engine.startup()) {
        CHAI_LOG_CRITICAL("Engine failed to start. Exiting prematurely.");
        return 1;
    }

    setupDefaultScene(engine.scene(),
                      engine.services().resolve<gfx::IModelRegistry>(),
                      engine.services().resolve<gfx::ITextureRegistry>());

    // auto viewportReg = engine.services().tryResolve<gfx::IViewportRegistry>();
    // auto id = viewportReg->addViewport("scene", engine.scene().getCameraId());

    engine.run([](const UpdateContext&) {});

    engine.shutdown();

    return 0;
}
