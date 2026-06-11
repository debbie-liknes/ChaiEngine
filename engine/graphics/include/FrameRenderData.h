#pragma once
#include <ChaiMath.h>
#include <Handle.h>

namespace chai::gfx
{
    struct Mesh;

    struct RenderObject 
    {
        Handle<Mesh> mesh;
        math::Mat4 model;
    };

    struct FrameRenderData 
    {
        math::Mat4 view;
        math::Mat4 proj;
        std::vector<RenderObject> objects;
    };
}