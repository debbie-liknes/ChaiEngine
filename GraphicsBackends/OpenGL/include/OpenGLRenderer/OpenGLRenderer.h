#pragma once
#include "ChaiEngine/PipelineState.h"

#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <memory>
#include <set>
#include <Types/CMap.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <Graphics/RenderKey.h>
#include <OpenGLRenderer/Uploads.h>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>
#include <OpenGLRenderer/OpenGLShader.h>
#include <OpenGLRenderer/OpenGLTexture.h>
#include <OpenGLRenderer/UniformManager.h>
#include <OpenGLRenderer/GLPipelineState.h>
#include <ChaiEngine/UniformBuffer.h>
#include <ChaiEngine/RenderFrame.h>
#include <Window/Window.h>
#include "RenderPipeline.h"

namespace chai::brew
{
    struct CommonUniforms
    {
        Mat4 view; // 64 bytes
        Mat4 projection; // 64 bytes
        Mat4 invView; // 64 bytes
    };

    struct DrawUniforms
    {
        Mat4 model;        // 64 bytes
        Mat4 normalMatrix; // 64 bytes
    };

    class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
    {
    public:
        OpenGLBackend();
        ~OpenGLBackend() override;

        bool initialize(std::unique_ptr<RenderSurface> surface, void* winProcAddress) override;
        void shutdown() override;

        // Main rendering entry point
        void executeCommands(const std::vector<RenderCommand>& commands) override;

        // Start/stop render thread
        bool startRenderThread();
        void stopRenderThread();

        // Wait for all pending frames to complete
        void waitForIdle();

        void bindShaderProgram(GLuint program);
        void drawBatchedCommands(const std::vector<SortedDrawCommand>& sortedDraws);

        // Statistics
        //const RenderStats& getStats() const { return m_stats; }
        //void printPerformanceStats();

        //render thread
        void renderThreadMain();
        void initializeRenderThread(); // Initialize GL context on render thread
        void shutdownRenderThread();

        // Execute frame on render thread (owns GL context)
        void executeFrame(const RenderFrame& frame);

        // Command execution
        RenderKey createSortKey(const RenderCommand& cmd, OpenGLMeshData* meshData);

        static void clear(float r, float g, float b, float a);

        // Uniform management
        void updatePerFrameUniforms();
        void updatePerDrawUniforms(const RenderCommand& cmd, const OpenGLShaderData* shaderData);
        void updateLightUniforms(const OpenGLShaderData* shaderData);

        // Material handling
        void applyMaterialState(OpenGLMaterialData* matData, OpenGLShaderData* shaderData);

        void setLights(const std::vector<LightInfo>& lights);

        GLShaderManager& getShaderManager() { return m_shaderManager; }
        OpenGLMeshManager& getMeshManager() { return m_meshManager; }
        OpenGLMaterialManager& getMaterialManager() { return m_matManager; }
        OpenGLTextureManager& getTextureManager() { return m_texManager; }
        UniformManager& getUniformManager() { return m_uniManager; }
        UploadQueue& getUploadQueue() { return m_uploadQueue; }
        GLPipelineState& getCurrentState() { return m_currentState; }

    private:
        //threading
        std::thread m_renderThread;
        std::atomic<bool> m_running{false};
        std::atomic<bool> m_initialized{false};

        // Double-buffered frame queue
        // Main thread writes to one, render thread reads from other
        std::mutex m_queueMutex;
        std::condition_variable m_queueCV;
        std::queue<RenderFrame> m_frameQueue;
        static constexpr size_t MAX_QUEUED_FRAMES = 2;

        // Frame synchronization
        std::mutex m_frameMutex;
        std::condition_variable m_frameCompleteCV;
        uint64_t m_currentFrameNumber{0};
        uint64_t m_lastCompletedFrame{0};

        // Everything under here is only accessed by the render thread
        UniformManager m_uniManager;
        OpenGLMeshManager m_meshManager;
        OpenGLMaterialManager m_matManager;
        OpenGLTextureManager m_texManager;
        GLShaderManager m_shaderManager;

        // Upload system
        UploadQueue m_uploadQueue;

        // Default resources
        GLuint defaultShaderProgram = 0;

        // Uniform Buffer Objects
        std::shared_ptr<UniformBuffer<CommonUniforms>> m_perFrameUBOData;
        std::shared_ptr<UniformBuffer<DrawUniforms>> m_perDrawUBOData;
        std::shared_ptr<UniformBuffer<LightingData>> m_lightingUBO;

        // State tracking (minimize redundant state changes)
        GLuint currentShaderProgram = 0;
        GLuint currentVAO = 0;

        // Cached data
        std::vector<LightInfo> m_cachedLights;
        bool m_lightsDirty = true;

        // Camera data (for sorting)
        Vec3 m_cameraPosition;

        // Frame timing
        float m_currentTime = 0.0f;
        float m_deltaTime = 0.0f;

        // Statistics
        //RenderStats m_stats;

        //pipeline state
        GLPipelineState m_currentState;
        RenderPipeline m_renderPipeline;

        void* m_winProcAddress = nullptr;
        std::unique_ptr<RenderSurface> m_surface;

        std::atomic<bool> m_threadExited{false};
    };
}

CHAI_PLUGIN_CLASS(OpenGLPlugin)
CHAI_DECLARE_PLUGIN_ENTRY(OPENGLRENDERER_EXPORT)