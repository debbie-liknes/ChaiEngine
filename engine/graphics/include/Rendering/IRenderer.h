/**
 * @file Renderer.h
 * @brief Interface for Renderer service
 */
#pragma once
#include <Handle.h>
#include <Rendering/FrameRenderData.h>

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
        };
    }
} // namespace chai