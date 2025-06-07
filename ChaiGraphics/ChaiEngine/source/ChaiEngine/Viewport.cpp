#include <ChaiEngine/Viewport.h>


namespace chai
{
	Viewport::Viewport()
	{
		//m_camera->SetPerspective(45.f, 0.1f, 100.f);
		//m_camera->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
		//m_camera->lookAt(glm::vec3(0.f, 0.f, 0.f));
	}

	Viewport::Viewport(int posX, int posY, int width, int height)
		: x(posX), y(posY), width(width), height(height)
	{

	}
}