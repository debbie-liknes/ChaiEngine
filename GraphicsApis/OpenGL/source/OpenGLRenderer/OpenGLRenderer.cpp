#include <OpenGLRenderer/OpenGLRenderer.h>
#include <glad/glad.h>
#include <iostream>
#include <ChaiGraphics/Viewport.h>

namespace CGraphics
{
	OpenGLBackend::OpenGLBackend()
	{
	}

	void OpenGLBackend::setProcAddress(void* address)
	{
		//call init on the graphics api instead of doing this here
		if (!gladLoadGLLoader((GLADloadproc)address))
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
		}
	}

	void OpenGLBackend::renderFrame(Window* window)
	{
		for (auto& v : window->GetViewports())
		{
			int x, y, width, height;
			v->GetDimensions(x, y, width, height);
			glViewport(x, y, width, height);
		}
	}
}