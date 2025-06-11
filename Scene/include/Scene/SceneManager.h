#pragma once
#include <Scene/Scene.h>

namespace chai::cup
{
	class SceneManager
	{
	public:
		SceneManager();
		void loadScene(const std::string& name);
		Scene* getCurrentScene() { return activeScene; }

	private:
		std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
		Scene* activeScene = nullptr;
	};
}