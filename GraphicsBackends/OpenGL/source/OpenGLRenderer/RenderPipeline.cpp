#include <OpenGLRenderer/RenderPipeline.h>
#include <OpenGLRenderer/ShadowPass.h>

namespace chai::brew
{
    RenderPipeline::~RenderPipeline() = default;

    void RenderPipeline::initialize(OpenGLBackend* backend, int width, int height)
    {
        m_backend = backend;

        // Create passes in order
        m_gbufferPass = std::make_unique<GBufferPass>();
        m_shadowPass = std::make_unique<ShadowPass>();
        m_lightingPass = std::make_unique<LightingPass>(m_gbufferPass.get(), m_shadowPass.get());
        // m_forwardPass = std::make_unique<ForwardPass>(m_gbufferPass.get());
        m_skyboxPass = std::make_unique<SkyboxPass>(m_gbufferPass.get());

        // Setup all passes
        m_gbufferPass->setup(backend);
        m_lightingPass->setup(backend);
        // m_forwardPass->setup(backend);
        m_skyboxPass->setup(backend);
        m_shadowPass->setup(backend);

        resize(width, height);
    }

    void RenderPipeline::shutdown()
    {
        // Passes will be destroyed automatically
        //m_gbufferPass.reset();
        //m_lightingPass.reset();
        // m_forwardPass.reset();
        // m_skyboxPass.reset();
    }

    void RenderPipeline::resize(int width, int height)
    {
        m_gbufferPass->resize(width, height);
        m_lightingPass->resize(width, height);
        m_skyboxPass->resize(width, height);
        m_shadowPass->resize(width, height);
        // m_forwardPass->resize(width, height);
    }

    void RenderPipeline::execute(
        const std::vector<SortedDrawCommand>& opaqueDraws,
        const std::vector<SortedDrawCommand>& transparentDraws,
        const std::vector<SortedDrawCommand>& skyboxDraws,
        const std::vector<LightInfo>& lights,
        const Mat4& cameraView,
        const Mat4& cameraProj,
        float nearPlane,
        float farPlane)
    {
        // Map the shadows first
        m_shadowPass->execute(m_backend, lights, opaqueDraws,
                      cameraView, cameraProj, nearPlane, farPlane);

        // 1. G-Buffer pass
        m_gbufferPass->execute(m_backend, opaqueDraws);

        // 2. Lighting pass
        m_lightingPass->execute(m_backend, lights);

        // 3. Skybox pass
        m_skyboxPass->execute(m_backend, skyboxDraws);

        // 4. Forward pass - transparent objects
        // m_forwardPass->execute(m_backend, transparentDraws);

        // 5. Post-processing passes (bloom, tone mapping, etc.)
        // ...
    }
}