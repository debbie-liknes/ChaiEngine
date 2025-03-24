#include "Chai.h"
#include <ChaiEngine/Engine.h>

using namespace std;

int main()
{
	//this is the rendering engine
	chai::brew::Engine engine;
	engine.init("OpenGLRenderer");
	engine.createWindow("Chai Window");

	//TODO: engine should only responsible for rendering what we give it, not the main loop
	engine.run();

	return 0;
}
