#include "Chai.h"

#include "Graphics/TextureAsset.h"

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
#include "Scene/Skybox.h"
using namespace chai::cup;

using namespace std;

const chai::Vec3 WORLD_UP{0.0f, 1.0f, 0.0f};

int main()
{
    //load common plugins
    chai::kettle::PluginRegistry::instance().loadPluginsInDirectory("plugins");

    chai::AssetManager::instance().addSearchPath("./assets");
    chai::AssetManager::instance().addSearchPath("./resources");
    chai::AssetManager::instance().addSearchPath(RESOURCE_PATH);
    // For development, maybe add the source directory
    //chai::AssetManager::instance().addSearchPath(PROJECT_SOURCE_DIR "/assets");

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
    auto surface = windowSystem->createRenderSurface(windowManager->getSystemWindow(mainWindow));

    renderer->initialize(std::move(surface), windowSystem->getProcAddress());

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

    auto modelAsset = chai::AssetManager::instance().load<chai::ModelAsset>("assets/Sponza/glTF/Sponza.gltf");
    auto sponza = testScene->createModelObject("SponzaRoot", modelAsset.value());
    sponza->getComponent<chai::cup::TransformComponent>()->setRotation(
        chai::Quat::fromEulerZYX(chai::radians(50.0f), chai::radians(50.f), chai::radians(50.f)));
    sponza->getComponent<chai::cup::TransformComponent>()->setScale(chai::Vec3(0.05, 0.05, 0.05));

    //add a camera to look through
    auto cameraObject = std::make_unique<chai::cup::GameObject>();
    auto* camComponent = cameraObject->addComponent<chai::cup::CameraComponent>(cameraObject.get());
    auto camTransform = cameraObject->getComponent<chai::cup::TransformComponent>();
    camTransform->setPosition(chai::Vec3{0.0, 10.0, 15.0});
    camTransform->lookAt(chai::Vec3{0.0, 0.0, 0.0}, WORLD_UP);
    cameraObject->addController<chai::cup::CameraController>();

    //add some lighting so we can see
    auto lightObject = std::make_unique<chai::cup::GameObject>();
    lightObject->getComponent<chai::cup::TransformComponent>()->setPosition(
        chai::Vec3{-5.0, 15.0, 3.0});
    //lightObject->getComponent<chai::cup::TransformComponent>()->lookAt(chai::Vec3{0.0, 0.0, 0.0},
     //   WORLD_UP);
    auto lightComp = lightObject->addComponent<chai::cup::LightComponent>(lightObject.get());
    lightComp->intensity = 600.f;
    lightComp->attenuation = chai::Vec3{1.0f, 0.045f, 0.0075f};
    lightComp->range = 500.0f;

    //add another
    auto lightObject2 = std::make_unique<chai::cup::GameObject>();
    lightObject2->getComponent<chai::cup::TransformComponent>()->setPosition(
        chai::Vec3{-5.0, 5.0, 3.0});
    //lightObject2->getComponent<chai::cup::TransformComponent>()->lookAt(chai::Vec3{0.0, 0.0, 0.0},
    //                                                                   WORLD_UP);
    auto lightComp2 = lightObject2->addComponent<chai::cup::LightComponent>(lightObject2.get());
    lightComp2->intensity = 60.f;

    auto sun = testScene->createGameObject("Sun");
    auto* light = sun->addComponent<LightComponent>();
    light->type = LightType::DIRECTIONAL;
    light->color = chai::Vec3(1.0f, 0.95f, 0.9f);
    light->intensity = 5.0f; // Bright!
    sun->getComponent<TransformComponent>()->lookAt(chai::Vec3{0.0, 0.0, 0.0},
                                                                       WORLD_UP);

    //set up viewport camera association
    vp->setCamera(camComponent->getCamera());

    //add the objects to the scene
    //testScene->addGameObject(std::make_unique<chai::cup::Skybox>());
    testScene->addGameObject(std::move(cameraObject));
    testScene->addGameObject(std::move(lightObject));
    //testScene->addGameObject(std::move(lightObject2));

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