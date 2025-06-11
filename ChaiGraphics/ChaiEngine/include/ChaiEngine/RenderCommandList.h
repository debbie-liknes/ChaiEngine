#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

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
        std::shared_ptr<IMesh> mesh;
        std::shared_ptr<IMaterial> material;
        glm::mat4 modelMatrix;

        // Other command data
        struct ViewportData {
            int x, y, width, height;
        } viewport;

        struct ClearData {
            float color[4];
            bool clearColor, clearDepth, clearStencil;
        } clear;
    };
}