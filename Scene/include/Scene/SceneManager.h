#pragma once
#include <Scene/Scene.h>

namespace chai::cup
{
	class SceneManager
	{
	public:
		SceneManager();

		void addScene(const std::string& name, std::unique_ptr<Scene> scene);
		Scene* setActiveScene(const std::string& name);
		Scene* getPrimaryScene() { return m_activeScene; }

	private:
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
		Scene* m_activeScene = nullptr;
	};
}