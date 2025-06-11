#include <Scene/Scene.h>
#include <ChaiEngine/Spherical.h>

namespace chai::cup
{
	void Scene::addGameObject(std::unique_ptr<GameObject> object)
	{
		m_objects.push_back(std::move(object));
	}
}