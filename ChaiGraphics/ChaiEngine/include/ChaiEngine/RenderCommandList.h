#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <Graphics/IViewport.h>
#include <glm/glm.hpp>

namespace chai::brew
{
    struct CHAIGRAPHICS_EXPORT RenderCommand {
        enum Type {
            DRAW_MESH,
            SET_VIEWPORT,
            CLEAR,
            SET_SCISSOR
        };

        Type type;

        // Mesh rendering data
        IMesh* mesh;
        std::shared_ptr<IMaterial> material;
        glm::mat4 transform;
        IViewport* viewport;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;

    };
}