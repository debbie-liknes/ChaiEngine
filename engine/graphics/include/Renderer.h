#pragma once

namespace chai
{
    class IWindow;
    namespace gfx
    {
        // The COARSE renderer seam — deliberately graphics-API-free, so it lives in
        // Core and the engine can call it without depending on Vulkan. It exposes
        // only frame lifecycle, nothing about how rendering works internally.
        //
        // This is NOT the backend abstraction. The rich stuff (render targets,
        // command recording, pipelines) stays concrete inside the Vulkan plugin
        // until a second backend forces a real abstraction out of the diff. A
        // graphics backend is just a plugin that provides an IRenderer.
        class IRenderer
        {
        public:
            virtual ~IRenderer() = default;

            // Produce one frame: acquire a target, render the scene into it, present.
            virtual void renderFrame() = 0;

            // Window was resized — rebuild anything sized to it (the swapchain).
            virtual void onResize(int width, int height) = 0;

            // Block until the GPU is idle. Call before tearing the renderer down,
            // so resources aren't destroyed while still in use by in-flight work.
            virtual void waitIdle() = 0;
        };
    }
} // namespace chai