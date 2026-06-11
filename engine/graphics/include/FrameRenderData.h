/**
 * @file FrameRenderData.h
 */
#pragma once
#include <ChaiMath.h>
#include <Handle.h>

namespace chai::gfx
{
    struct Mesh;

    /**
     * @brief This is probably temporary, in its current form
     */
    struct RenderObject 
    {
        Handle<Mesh> mesh;
        math::Mat4 model;
    };

    /**
     * @brief Per frame data needed by the renderer
     */
    struct FrameRenderData 
    {
        math::Mat4 view;
        math::Mat4 proj;
        std::vector<RenderObject> objects;
    };
}