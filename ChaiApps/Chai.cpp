//test exe

#include "Chai.h"
#include <ChaiEngine/ChaiWindow.h>
#include <Engine/Viewport.h>
#include <OpenGLRenderer/OpenGLRenderer.h>
#include <RenderObjects/Cube.h>

using namespace std;

int main()
{
	//TODO:make this a generic window
	CGraphics::ChaiWindow window("Chai Engine");
	CGraphics::Renderer* renderer = new CGraphics::OpenGLBackend();
	CGraphics::SharedViewport testViewport = std::make_shared<CGraphics::Viewport>(0, 0, window.GetWidth(), window.GetHeight());

	window.AddViewport(testViewport);
	renderer->setProcAddress(window.getProcAddress());

	//test ro
	auto triangle = std::make_shared<CGraphics::CubeRO>();
	Core::CVector<Core::CSharedPtr<CGraphics::RenderObject>> ros = { triangle };

	while (window.Show())
	{
		window.swapBuffers();
		window.PollEvents();
		renderer->renderFrame(&window, ros);

	}

	window.Close();

	delete renderer;
	return 0;
}
