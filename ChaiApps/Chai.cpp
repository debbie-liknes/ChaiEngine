//test exe

#include "Chai.h"
#include <Window/GLFWImpl.h>
#include <Window/Viewport.h>
#include <OpenGLRenderer/OpenGLRenderer.h>
#include <RenderObjects/Cube.h>
#include <ChaiEngine/ViewData.h>
#include <Scene/Camera.h>

using namespace std;

int main()
{
	//TODO:make this a generic window
	CGraphics::ChaiWindow window("Chai Engine");
	chai_graphics::Renderer* renderer = new chai_graphics::OpenGLBackend();
	CGraphics::SharedViewport testViewport = std::make_shared<CGraphics::Viewport>(0, 0, window.GetWidth(), window.GetHeight());
	//CScene::Camera camera;

	window.AddViewport(testViewport);
	renderer->setProcAddress(window.getProcAddress());

	chai_graphics::ViewData data;
	data.view = glm::mat4(1.f);

	float fov = glm::radians(45.0f);
	float aspect = 16.0f / 9.0f;
	float near = 0.1f;
	float far = 100.0f;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cameraPos, target, up);
	glm::mat4 projection = glm::perspective(fov, aspect, near, far);
	data.projMat = projection;


	//test ro
	auto triangle = std::make_shared<chai_graphics::CubeRO>();
	Core::CVector<Core::CSharedPtr<chai_graphics::RenderObject>> ros = { triangle };

	while (window.Show())
	{
		window.swapBuffers();
		window.PollEvents();
		renderer->renderFrame(&window, ros, data);
	}

	window.Close();

	delete renderer;
	return 0;
}
