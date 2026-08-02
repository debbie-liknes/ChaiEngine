#pragma once
#include <Handle.h>
#include <string>
#include <ChaiMath.h>

namespace chai::gfx
{
    struct Viewport;

    class ViewportHandle : public Handle<Viewport>
    {
    };

    enum class ViewportShadingMode : int { Lit = 0, Normals = 1, UV = 2, Overdraw = 3 };

    class IViewportRegistry
    {
    public:
        // Viewport management
        virtual ViewportHandle addViewport(const std::string& id, uint32_t cameraViewIndex) = 0;
        virtual void removeViewport(ViewportHandle handle) = 0;
        virtual void
        requestViewportResize(ViewportHandle handle, uint32_t width, uint32_t height) = 0;
        virtual uint64_t getViewportTextureId(ViewportHandle handle) const = 0;
        virtual void setViewportHovered(ViewportHandle handle, bool hovered) = 0;
        virtual math::Vec2 getViewportExtent(ViewportHandle handle) const = 0;

        // debugging tools
        virtual void setViewportWireframe(ViewportHandle handle, bool enabled) = 0;
        virtual bool isViewportWireframe(ViewportHandle handle) const = 0;
        virtual void setViewportShadingMode(ViewportHandle handle, ViewportShadingMode mode) = 0;
        virtual ViewportShadingMode getViewportShadingMode(ViewportHandle handle) const = 0;
    };
} // namespace chai::gfx