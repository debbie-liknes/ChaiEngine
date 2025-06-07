#pragma once
#include <vector>
#include <Scene/Light.h>
#include <Scene/Camera.h>
#include <Scene/Entity.h>

namespace chai::cup
{
	//Scene class that holds all the entities in the scene
	//Does not hold the camera, those are associated with views (probably players?)
	//The scene should hold data that is persistent across frames
    class Scene {
    public:
        Scene()
        {
            init();
        }
        void init();

        std::vector<Entity> m_entities;
		std::vector<Light> m_lights;

    private:
    };
}