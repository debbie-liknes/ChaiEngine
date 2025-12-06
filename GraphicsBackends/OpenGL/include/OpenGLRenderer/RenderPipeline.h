#pragma once
#include "ShadowPass.h"

#include <Graphics/RenderKey.h>
#include <OpenGLRenderer/GBuffPass.h>
#include <OpenGLRenderer/LightingPass.h>
#include <OpenGLRenderer/SkyboxPass.h>
#include <memory>
#include <vector>

namespace chai::brew
{
    class OpenGLBackend;
    class LightingPass;

    class RenderPipeline
    {
    public:
        RenderPipeline() = default;
        ~RenderPipeline(); // declaration only

        RenderPipeline(const RenderPipeline&) = delete;
        RenderPipeline& operator=(const RenderPipeline&) = delete;
        RenderPipeline(RenderPipeline&&) = delete;
        RenderPipeline& operator=(RenderPipeline&&) = delete;

        void initialize(OpenGLBackend* backend, int width, int height);
        void shutdown();
        void resize(int width, int height);

        void execute(const std::vector<SortedDrawCommand>& opaqueDraws,
                     const std::vector<SortedDrawCommand>& transparentDraws,
                     const std::vector<SortedDrawCommand>& skyboxDraws,
                     const std::vector<LightInfo>& lights);

    private:
        OpenGLBackend* m_backend = nullptr;
        std::unique_ptr<GBufferPass> m_gbufferPass;
        std::unique_ptr<LightingPass> m_lightingPass;
        // std::unique_ptr<ForwardPass> m_forwardPass;
        std::unique_ptr<SkyboxPass> m_skyboxPass;
        std::unique_ptr<ShadowPass> m_shadowPass;
    };
} // namespace chai::brew
