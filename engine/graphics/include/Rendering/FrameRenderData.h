/**
 * @file FrameRenderData.h
 */
#pragma once
#include <ChaiMath.h>
#include <Handle.h>

namespace chai::gfx
{
    struct Mesh;
    struct Texture;

    struct RenderView { // one per camera/view
        math::Mat4 view, proj, viewProj;
        math::Vec3 position;
    };

    struct RenderItem { // one per drawable
        Handle<Mesh> mesh;
        math::Mat4 model;
        //this will be material someday
        math::Vec4 color;
        Handle<Texture> texture;
        uint32_t materialId = 0;
    };

    //This represents ONE world view
    //if we have separate, unrelated panes, we'll need more of these
    struct FrameRenderData 
    {
        std::vector<RenderView> views;
        std::vector<RenderItem> items;
    };
}