#include <Scene/Scene.h>
#include <ChaiEngine/Spherical.h>

namespace chai::cup
{
	void Scene::init()
	{
		Entity ent;
		m_entities.push_back(ent);

		Light light;
		light.color = { 1.f, 1.f, 1.f };
		light.position = { 5.f, 10.f, 0.f };
		light.intensity = 5.f;
		m_lights.push_back(light);
	}
}