//test exe

#include "Chai.h"
#include <ChaiGraphics/ChaiWindow.h>
#include <OpenGLRenderer/OpenGLRenderer.h>

using namespace std;

int main()
{
	CGraphics::ChaiWindow window("Chai Engine");
	CGraphics::IRenderer renderer = CGraphics::OpenGLBackend();
	

	while (window.Show())
	{
		window.PollEvents();
	}

	window.Close();
	return 0;
}
