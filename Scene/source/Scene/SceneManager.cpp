#include <Scene/SceneManager.h>

namespace chai::cup
{
	SceneManager::SceneManager()
	{
	}

	Scene* SceneManager::setActiveScene(const std::string& name)
	{
		m_activeScene = m_scenes[name].get();
		return m_activeScene;
	}

	void SceneManager::addScene(const std::string& name, std::unique_ptr<Scene> scene)
	{
		m_scenes.insert({ name, std::move(scene) });
	}

	void SceneManager::update(double deltaTime)
	{
		for (auto&& scene : m_scenes)
		{
			scene.second->update(deltaTime);
		}
	}
}