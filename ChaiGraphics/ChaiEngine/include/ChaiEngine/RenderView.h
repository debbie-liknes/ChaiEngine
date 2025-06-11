#pragma once
#include <Renderables/Renderable.h>
#include <ChaiEngine/ViewData.h>

namespace chai::brew
{
    class Camera;
    //constructed per frame 
    struct RenderView 
    {
        Camera* camera;
        //Viewport* viewport;
		ViewData viewData;
    };
}