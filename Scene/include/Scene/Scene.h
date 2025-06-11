#pragma once
#include <vector>
#include <Scene/Light.h>
#include <Scene/Camera.h>
#include <Scene/GameObject.h>

namespace chai::cup
{
	//Scene class that holds all the entities in the scene
	//Does not hold the camera, those are associated with views (probably players?)
	//The scene should hold data that is persistent across frames
    class Scene {
    public:
        Scene() = default;
        ~Scene() = default;

        void addGameObject(std::unique_ptr<GameObject> object);
    private:

        std::vector<std::unique_ptr<GameObject>> m_objects;

    private:
    };
}