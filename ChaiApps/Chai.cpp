﻿#include "Chai.h"
#include <Engine/Engine.h>
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
#include <ChaiEngine/AssetManager.h>
#include <Components/TransformComponent.h>
#include <Input/InputState.h>
#include <Controllers/CameraController.h>
#include <chrono>

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
	uint32_t mainVpId = viewportManager.createViewport(mainWindow, { "MainView", 0, 0, 1080, 720 });
	chai::Viewport* vp = viewportManager.getViewport(mainVpId);

	//make a scene
	chai::cup::SceneManager sceneManager;
	sceneManager.addScene("TestScene", std::make_unique<chai::cup::Scene>());
	chai::cup::Scene* testScene = sceneManager.setActiveScene("TestScene");

	//make an object for the scene
	auto gameObject = std::make_unique<chai::cup::GameObject>();
	chai::cup::MeshComponent* meshComp = gameObject->addComponent<chai::cup::MeshComponent>(gameObject.get());
	meshComp->setMesh(chai::brew::AssetManager::instance().loadMesh("assets/suzanne.obj")->getMesh());
	gameObject->getComponent<chai::cup::TransformComponent>()->setPosition({ 0.0, 0.0, 0.0 });

	//add a camera to look through
	auto cameraObject = std::make_unique<chai::cup::GameObject>();
	chai::cup::CameraComponent* camComponent = cameraObject->addComponent<chai::cup::CameraComponent>(cameraObject.get());
	auto camTransform = cameraObject->getComponent<chai::cup::TransformComponent>();
	camTransform->setPosition({ -5.0, 0.0, 10.0 });
	camTransform->lookAt({ -2.4941, 1.3559, 4.7810 }, {0.0, 1.0, 0.0});
	camComponent->getViewMatrix();
	cameraObject->addController<chai::cup::CameraController>();

	//add some lighting so we can see
	auto lightObject = std::make_unique<chai::cup::GameObject>();
	lightObject->getComponent<chai::cup::TransformComponent>()->setPosition({ -5.0, 5.0, -5.0 });
	lightObject->getComponent<chai::cup::TransformComponent>()->lookAt({ 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
	lightObject->addComponent<chai::cup::LightComponent>(lightObject.get());

	//set up viewport camera association
	vp->setCamera(camComponent->getCamera());

	//add the objects to the scene
	testScene->addGameObject(std::move(gameObject));
	testScene->addGameObject(std::move(cameraObject));
	testScene->addGameObject(std::move(lightObject));


	// Time tracking for delta time
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!windowManager->isDone())
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		chai::InputSystem::instance().processEvents();
		sceneManager.update(deltaTime);

		//render loop
		for (auto& viewport : viewportManager.getAllViewports()) 
		{
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
			if (scene) 
			{
				scene->collectRenderables(collector);
			}


			renderer->executeCommands(collector.getCommands());
		}

		//updates and buffer swap
		windowManager->update();
		camComponent->update(deltaTime);
	}

	renderer->shutdown();

	return 0;
}
