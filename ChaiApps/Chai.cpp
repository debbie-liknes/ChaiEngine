//test exe

#include "Chai.h"
#include <RenderObjects/Cube.h>
#include <Scene/Camera.h>
#include <ChaiEngine/Engine.h>

using namespace std;

int main()
{
	//this is the rendering engine
	chai::brew::Engine engine;
	engine.init("OpenGLRenderer");
	engine.createWindow("Chai Window");

	//TODO:make this a generic window
	//CScene::Camera camera;

	engine.run();

	return 0;
}
