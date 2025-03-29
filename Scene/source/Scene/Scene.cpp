#include <Scene/Scene.h>

namespace chai::cup
{
	void Scene::init()
	{
		m_cam.SetPerspective(30.f, 0.1f, 100.f);
		m_cam.SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
		m_cam.lookAt(glm::vec3(0.f, 0.f, 0.f));

		Entity ent;
		m_entities.push_back(ent);
	}
}