#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/Viewport.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::brew
{
    class CHAIGRAPHICS_EXPORT RenderCommandList {
    public:
        virtual ~RenderCommandList() = default;
        virtual void setViewport(const Viewport& viewport) = 0;
        //virtual void Clear(const Color& color) = 0;
        virtual void drawMesh(const IMesh& mesh, const IMaterial& material) = 0;
        //virtual void SetRenderTarget(RenderTarget* target) = 0;
    };
}