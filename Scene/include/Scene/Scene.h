#pragma once
#include <vector>
#include <Scene/Light.h>
#include <Scene/Camera.h>
#include <Scene/Entity.h>

namespace chai::cup
{
    class Scene {
    public:
        Scene()
        {
            init();
        }
        void init();
        //having flatlists is temporary
        //need a tree of everything in the scene
        std::vector<Light> m_lights;
        Camera m_cam;
        std::vector<Entity> m_entities;

    private:
    };
}