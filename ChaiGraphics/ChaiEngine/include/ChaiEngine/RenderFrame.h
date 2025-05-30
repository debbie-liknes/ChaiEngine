#pragma once
#include <Renderables/Renderable.h>
#include <Window/Viewport.h>

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

    struct RenderFrame {
        GPUCamera camera;
        std::vector<GPULight> lights;
        std::vector<std::shared_ptr<Renderable>> renderables;
        float time;
        Viewport* viewport;
    };
}