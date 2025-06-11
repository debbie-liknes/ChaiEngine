#pragma once
#include <Renderables/Renderable.h>
#include <ChaiEngine/RenderView.h>

namespace chai::brew
{
    struct GPULight {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    };

    struct GPUCamera {
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec3 position;
        //redundant, since you can extract from view mat, if you want
        glm::vec3 forward;
        glm::vec3 up;
    };

    class Light;
    struct RenderFrame 
    {
        //a flatlist probably is a great idea. Layout manager or something
        std::vector<RenderView> views;
        std::vector<Light*> lights;
		std::vector<Renderable*> renderables;
    };
}