/**
 * @file FrameRenderData.h
 */
#pragma once
#include <GraphicsExport.h>
#include <ChaiMath.h>
#include <Handle.h>
#include <Rendering/LightData.h>
#include <Rendering/EnvironmentData.h>

namespace chai::gfx
{
    struct Mesh;
    struct Texture;
    struct Material;

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
        Handle<Material> material;
    };

    //This represents ONE world view
    //if we have separate, unrelated panes, we'll need more of these
    struct GRAPHICS_EXPORT FrameRenderData 
    {
        std::vector<RenderView> views;
        std::vector<RenderItem> items;
        LightData sun;                  //have 1 directional light
        EnvironmentData environment;
    };
}