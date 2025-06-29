#pragma once
#include <Scene/Scene.h>
#include <Core/Updatable.h>

namespace chai::cup
{
	class SceneManager : public IUpdatable
	{
	public:
		SceneManager();

		void addScene(const std::string& name, std::unique_ptr<Scene> scene);
		Scene* setActiveScene(const std::string& name);
		Scene* getPrimaryScene() { return m_activeScene; }

		void update(double deltaTime) override;

	private:
		std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
		Scene* m_activeScene = nullptr;
	};
}