#include <Scene/Scene.h>
#include <ChaiEngine/Spherical.h>

namespace chai::cup
{
	void Scene::addGameObject(std::unique_ptr<GameObject> object)
	{
		m_objects.push_back(std::move(object));
	}

	void Scene::collectRenderables(brew::RenderCommandCollector& collector) const
	{
		//do this in a scene graph traversal?
		for(auto& object : m_objects)
		{
			object->collectRenderables(collector);
		}
	}
}