#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <Graphics/IViewport.h>
#include <ChaiEngine/Light.h>
#include <glm/glm.hpp>

namespace chai::brew
{
    struct CHAIGRAPHICS_EXPORT RenderCommand 
    {
        enum Type {
            DRAW_MESH,
            SET_VIEWPORT,
            CLEAR,
            SET_SCISSOR,
            SET_LIGHTS
        };

        Type type;

        // Mesh rendering data
        IMesh* mesh;
        IMaterial* material;
        glm::mat4 transform;
        IViewport* viewport;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
        std::vector<Light> lights;

    };
}