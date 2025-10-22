#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <Graphics/Viewport.h>
#include <ChaiEngine/Light.h>

namespace chai::brew
{
    struct CHAIGRAPHICS_EXPORT RenderCommand 
    {
        enum Type 
        {
            DRAW_MESH,
            SET_VIEWPORT,
            CLEAR,
            SET_SCISSOR,
            SET_LIGHTS
        };

        Type type{ Type::CLEAR };

        // Mesh rendering data
        IMesh* mesh = nullptr;
        IMaterial* material = nullptr;
        Mat4 transform{ 1.f };
        IViewport* viewport = nullptr;
        Mat4 viewMatrix{ 1.f };
        Mat4 projectionMatrix{ 1.f };
        std::vector<Light> lights;

    };
}