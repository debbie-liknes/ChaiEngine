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
	chai::ChaiWindow window("Chai Engine");
	chai::brew::Renderer* renderer = new chai::brew::OpenGLBackend();
	chai::SharedViewport testViewport = std::make_shared<chai::Viewport>(0, 0, window.GetWidth(), window.GetHeight());
	//CScene::Camera camera;

	window.AddViewport(testViewport);
	renderer->setProcAddress(window.getProcAddress());

	chai::brew::ViewData data;
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
	auto triangle = std::make_shared<chai::brew::CubeRO>();
	chai::CVector<chai::CSharedPtr<chai::brew::RenderObject>> ros = { triangle };

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
