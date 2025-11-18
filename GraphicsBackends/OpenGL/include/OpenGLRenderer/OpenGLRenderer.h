#pragma once
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <memory>
#include <set>
#include <Types/CMap.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <ChaiEngine/Material.h>
#include <OpenGLRenderer/RenderKey.h>
#include <OpenGLRenderer/Uploads.h>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>
#include <OpenGLRenderer/OpenGLShader.h>
#include <OpenGLRenderer/OpenGLTexture.h>
#include <OpenGLRenderer/UniformManager.h>
#include <ChaiEngine/UniformBuffer.h>

namespace chai::brew
{
    struct CommonUniforms 
    {
        chai::Mat4 view;                  // 64 bytes
        Mat4 projection;            // 64 bytes
    };

    struct DrawUniforms 
    {
        Mat4 model;                 // 64 bytes
        Mat4 normalMatrix;          // 64 bytes
    };

	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
    {
    public:
        //OpenGLBackend() = default;

        bool initialize(void* winProcAddress) override;
        void shutdown() override;

        // Main rendering entry point
        void executeCommands(const std::vector<RenderCommand>& commands) override;

        // Frame lifecycle
        void beginFrame() override;
        //void endFrame();

        // Statistics
        //const RenderStats& getStats() const { return m_stats; }
        //void printPerformanceStats();

    private:
        // Initialization
        GLuint createDefaultShaderProgram();
        GLuint compileShader(const char* source, GLenum type);
        //void setupVertexAttributes();

        // Command execution
        RenderKey createSortKey(const RenderCommand& cmd, OpenGLMeshData* meshData);
        void drawBatchedCommands(const std::vector<SortedDrawCommand>& sortedDraws);

        // Drawing
        //void drawInstanced(const InstanceBatch& batch);
        //void setupInstancing(OpenGLMeshData* meshData, uint32_t maxInstances);

        // State management
        void bindShaderProgram(GLuint program);
        void bindVertexArray(GLuint vao);
        void clear(float r, float g, float b, float a);

        // Uniform management
        void updatePerFrameUniforms();
        void updatePerDrawUniforms(const RenderCommand& cmd, OpenGLShaderData* shaderData);
        void setLightsUniforms(OpenGLShaderData* shaderData);
        void setUniformValue(GLint location, const std::unique_ptr<UniformBufferBase>& uniform);

        // Material handling
        void applyMaterialState(OpenGLMaterialData* matData, OpenGLShaderData* shaderData);
        //void compileMaterial(Handle materialHandle, OpenGLMaterialData* glMaterialData);
        //GLuint compileShaderFromDescription(std::shared_ptr<ShaderDescription> shaderDesc,
        //    const std::set<MaterialFeature>& features);

        // Resource management
        //OpenGLShaderData* getShaderData(GLuint program);

        //void uploadMeshToGPU(Handle meshHandle, OpenGLMeshData* glMeshData);
        //void cacheBuiltinUniformLocations(GLuint program, OpenGLShaderData* shaderData);
        //void cacheUniformLocations(GLuint program, OpenGLMaterialData* matData);

        // Utility
        //std::string loadShaderFile(const std::string& filePath);
        //std::string injectFeatureDefines(const std::string& source,
        //    const std::set<MaterialFeature>& features);
        //std::string generateShaderHash(std::shared_ptr<ShaderDescription> desc,
        //    const std::set<MaterialFeature>& features);

    private:
        
        UniformManager m_uniManager;
		OpenGLMeshManager m_meshManager;
		OpenGLMaterialManager m_matManager;
		OpenGLTextureManager m_texManager;
		GLShaderManager m_shaderManager;

        void updateShader(OpenGLShaderData* program, uint32_t& stateChanges);

        // Upload system
        UploadQueue m_uploadQueue;

        // Default resources
        GLuint defaultShaderProgram = 0;

        // Uniform Buffer Objects
		std::shared_ptr<UniformBuffer<CommonUniforms>> m_perFrameUBOData;
		std::shared_ptr<UniformBuffer<DrawUniforms>> m_perDrawUBOData;
        GLuint m_lightingUBO = 0;

        // State tracking (minimize redundant state changes)
        GLuint currentShaderProgram = 0;
        GLuint currentVAO = 0;

        // Cached data
        std::vector<Light> m_cachedLights;
        bool m_lightsDirty = true;

        // Camera data (for sorting)
        Vec3 m_cameraPosition;

        // Frame timing
        float m_currentTime = 0.0f;
        float m_deltaTime = 0.0f;

        // Statistics
        //RenderStats m_stats;
    };
}

CHAI_PLUGIN_CLASS(OpenGLPlugin) 
{
	CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::OpenGLBackend, "Renderer")
}
CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLRenderer", "1.0.0")