/**
 * @file Renderer.h
 * @brief Interface for Renderer service
 */
#pragma once
#include <Handle.h>
#include <Rendering/FrameRenderData.h>
#include <Rendering/Viewport.h>
#include <string>

namespace chai
{
    class IWindow;
    namespace gfx
    {
        struct MeshAsset;   //cpu side
        struct Mesh;        //gpu side

        /**
         * @brief Deliberately graphics API free. Engine can use this without knowing if it
         * depends on vulkan, opengl etc
         */
        class IRenderer
        {
        public:
            virtual ~IRenderer() = default;

            /**
             * @brief Give the renderer the chance to do frame setup
             */
            virtual void startFrame() = 0;

            /**
             * @brief Give the renderer the chance to do frame cleanup
             */
            virtual void endFrame() = 0;

            /**
             * @brief Produces one frame.
             * Acquire a target, render to it, present it
             */
            virtual void renderFrame(const FrameRenderData& renderData) = 0;

            /**
             * @brief Tell the renderer the window was resized (swapchain)
             */
            virtual void onResize(int width, int height) = 0;

            /**
             * @brief Block until the gpu is idle. Used for teardown
             */
            virtual void waitIdle() = 0;


            //Viewport management
            virtual ViewportHandle addViewport(const std::string& id, uint32_t cameraViewIndex) = 0;
            virtual void removeViewport(ViewportHandle handle) = 0;
            virtual void
            requestViewportResize(ViewportHandle handle, uint32_t width, uint32_t height) = 0;
            virtual uint64_t getViewportTextureId(ViewportHandle handle) const = 0;
            virtual void setViewportHovered(ViewportHandle handle, bool hovered) = 0;
            virtual math::Vec2 getViewportExtent(ViewportHandle handle) const = 0;
        };
    }
} // namespace chai