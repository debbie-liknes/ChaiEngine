//test exe

#include "Chai.h"
#include <ChaiGraphics/ChaiWindow.h>
#include <ChaiGraphics/Viewport.h>
#include <OpenGLRenderer/OpenGLRenderer.h>

using namespace std;

int main()
{
	//TODO:make this a generic window
	CGraphics::ChaiWindow window("Chai Engine");
	CGraphics::IRenderer* renderer = new CGraphics::OpenGLBackend();
	CGraphics::SharedViewport testViewport = std::make_shared<CGraphics::Viewport>(0, 0, window.GetWidth(), window.GetHeight());

	window.AddViewport(testViewport);
	renderer->setProcAddress(window.getProcAddress());

	while (window.Show())
	{
		window.swapBuffers();
		window.PollEvents();
		renderer->renderFrame(&window);
	}

	window.Close();

	delete renderer;
	return 0;
}
