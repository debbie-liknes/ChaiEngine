#include "Chai.h"
#include <Window/WindowManager.h>
#include <Window/WindowSystem.h>
#include <Window/Window.h>
#include <Window/ViewportManager.h>
#include <Plugin/PluginRegistry.h>
#include <Plugin/ServiceLocator.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/RenderCommandCollector.h>
#include <Scene/GameObject.h>
#include <Scene/SceneManager.h>
#include <Scene/Scene.h>
#include <Components/MeshComponent.h>
#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Components/TransformComponent.h>
#include <Controllers/CameraController.h>
#include <chrono>
#include <AudioEngine.h>
#include <ChaiPhysics/ChaiPhysics.h>
#include <Asset/AssetHandle.h>
#include <ChaiEngine/MaterialSystem.h>

#include "Graphics/ShaderAsset.h"

using namespace std;

int main()
{
    //load common plugins
    chai::kettle::PluginRegistry::instance().loadPluginsInDirectory("plugins");

    //create window system and manager
    auto windowSystem = chai::ServiceLocator::instance().get<chai::WindowSystem>();
    auto windowManager = std::make_unique<chai::WindowManager>(windowSystem);
    chai::ViewportManager viewportManager;

    chai::WindowDesc desc;
    desc.title = "Chai Window";
    desc.width = 1080;
    desc.height = 720;
    desc.fullscreen = false;
    desc.vsync = true;
    desc.samples = 4; // 4x MSAA

    //create the top level OS container
    auto mainWindow = windowManager->createWindow(desc);

    //this is the renderer
    auto renderer = chai::ServiceLocator::instance().get<chai::brew::Renderer>();
    renderer->initialize(windowSystem->getProcAddress());

    //create viewports to go inside the window
    uint64_t mainVpId = viewportManager.createViewport(mainWindow, {"MainView", 0, 0, 1080, 720});
    chai::Viewport* vp = viewportManager.getViewport(static_cast<uint32_t>(mainVpId));

    // initialize physics engine
    chai::ChaiPhysics physicsEngine;
    physicsEngine.init();

    //make a scene
    chai::cup::SceneManager sceneManager;
    sceneManager.addScene("TestScene", std::make_unique<chai::cup::Scene>());
    chai::cup::Scene* testScene = sceneManager.setActiveScene("TestScene");

    //make an object for the scene
    auto gameObject = std::make_unique<chai::cup::GameObject>();
    auto* meshComp = gameObject->addComponent<chai::cup::MeshComponent>(gameObject.get());
    auto meshAsset = chai::AssetManager::instance().load<chai::MeshAsset>("assets/plane.obj");
    meshComp->setMesh(meshAsset.value());

    chai::MaterialSystem matSystem;

    auto resourceHandle = matSystem.createMaterialResourceFromAsset(matSystem.getDefaultAsset());

    auto materialInstance = std::make_unique<chai::MaterialInstance>(resourceHandle);

    // Customize instance
    materialInstance->setParameter("u_DiffuseColor", chai::Vec3(1.0f, 1.0f, 0.0f));
    meshComp->setMaterial(
        chai::ResourceManager::instance().add<chai::MaterialInstance>(std::move(materialInstance)));

    gameObject->getComponent<chai::cup::TransformComponent>()->setPosition(
        chai::Vec3{0.0, 0.0, 0.0});

    //add a camera to look through
    auto cameraObject = std::make_unique<chai::cup::GameObject>();
    auto* camComponent = cameraObject->addComponent<chai::cup::CameraComponent>(cameraObject.get());
    auto camTransform = cameraObject->getComponent<chai::cup::TransformComponent>();
    camTransform->setPosition(chai::Vec3{-5.0, 0.0, -10.0});
    camTransform->lookAt(chai::Vec3{0.f, 0.f, 0.f}, chai::Vec3{0.0, -1.0, 0.0});
    cameraObject->addController<chai::cup::CameraController>();

    //add some lighting so we can see
    auto lightObject = std::make_unique<chai::cup::GameObject>();
    lightObject->getComponent<chai::cup::TransformComponent>()->setPosition(
        chai::Vec3{-5.0, 5.0, -5.0});
    lightObject->getComponent<chai::cup::TransformComponent>()->lookAt(chai::Vec3{0.0, 0.0, 0.0},
        chai::Vec3{0.0, -1.0, 0.0});
    lightObject->addComponent<chai::cup::LightComponent>(lightObject.get());

    //set up viewport camera association
    vp->setCamera(camComponent->getCamera());

    //add the objects to the scene
    testScene->addGameObject(std::move(gameObject));
    testScene->addGameObject(std::move(cameraObject));
    testScene->addGameObject(std::move(lightObject));

    //audio
    //std::shared_ptr<AudioEngine> m_audioEngine;
    //m_audioEngine->Init();

    //uncomment for sound at location of cube
    //m_audioEngine->LoadSound(/*path to wav*/, true, true, true);
    //m_audioEngine->PlaySound(/*path to wav*/, glm::vec3{0,0,0}, 3.0F);

    // Time tracking for delta time
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!windowManager->isDone()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        chai::InputSystem::instance().processEvents();
        sceneManager.update(deltaTime);

        //render loop
        for (auto& viewport : viewportManager.getAllViewports()) {
            int x, y, w, h;
            viewport->getViewport(x, y, w, h);
            viewport->getCamera()->setAspectRatio(w / static_cast<float>(h));
            chai::brew::RenderCommandCollector collector;
            collector.setCamera(camComponent->getCamera());
            collector.setViewport(viewport);

            chai::brew::RenderCommand viewportCmd;
            viewportCmd.type = chai::brew::RenderCommand::SET_VIEWPORT;
            viewportCmd.viewport = viewport;
            collector.submit(viewportCmd);

            chai::brew::RenderCommand clearCmd;
            clearCmd.type = chai::brew::RenderCommand::CLEAR;
            collector.submit(clearCmd);

            //multiple scenes? additive scenes?
            chai::cup::Scene const* scene = sceneManager.getPrimaryScene();
            scene->collectLights(collector);
            if (scene != nullptr) {
                scene->collectRenderables(collector);
            }

            renderer->executeCommands(collector.getCommands());
            renderer->beginFrame();
        }

        //updates and buffer swap
        windowManager->update();
        camComponent->update(deltaTime);

        //audio update
        //m_audioEngine->Set3dListenerAndOrientation(
        //	camTransform->getWorldPosition(),
        //	camTransform->forward(),
        //	camTransform->up()
        //);
        //m_audioEngine->Update();
    }

    //m_audioEngine->Shutdown();
    renderer->shutdown();

    return 0;
}