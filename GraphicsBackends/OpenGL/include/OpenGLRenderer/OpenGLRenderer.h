#pragma once
#include <OpenGLRendererExport.h>
#include <glad/gl.h>
#include <Meta/ChaiMacros.h>
#include <ChaiEngine/Renderer.h>
#include <memory>
#include <OpenGLRenderer/OpenGLMesh.h>
#include <OpenGLRenderer/OpenGLMaterial.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>
#include <ChaiEngine/Material.h>
#include <set>
#include <Types/CMap.h>

namespace chai::brew
{
	class OPENGLRENDERER_EXPORT OpenGLBackend : public Renderer
	{
    public:
        OpenGLBackend() = default;
        virtual ~OpenGLBackend() = default;

        OpenGLBackend(const OpenGLBackend&) = delete;
        OpenGLBackend& operator=(const OpenGLBackend&) = delete;

        OpenGLBackend(OpenGLBackend&&) = default;
        OpenGLBackend& operator=(OpenGLBackend&&) = default;

        // RendererPlugin interface
        bool initialize(void* winProcAddress = nullptr) override;
        void shutdown() override;
        void executeCommands(const std::vector<RenderCommand>& commands) override;

    private:
        void applyMaterialState(Handle material, GLuint shaderProgram, OpenGLMaterialData* glMaterialData);
        OpenGLMaterialData* getOrCreateMaterialData(Handle material);
        void setUniformFromData(GLuint shaderProgram, const std::string& name, const UniformBufferBase& uniform);
        void compileMaterial(Handle material, OpenGLMaterialData* glMaterialData);
        void setBuiltinUniforms(GLuint shaderProgram, const RenderCommand& cmd);
        void unbindTextures(OpenGLMaterialData* glMaterialData);
        GLuint createDefaultShaderProgram();
        void bindShaderProgram(GLuint program);
        GLuint compileShader(const char* source, GLenum type);
        GLuint compileShaderFromDescription(std::shared_ptr<ShaderDescription> shaderDesc,
            const std::set<MaterialFeature>& features);
        std::string loadShaderFile(const std::string& filePath);
        std::string injectFeatureDefines(const std::string& source,
            const std::set<MaterialFeature>& features);
        void cacheUniformLocations(GLuint shaderProgram, OpenGLMaterialData* glMaterialData);
        void setLightsUniforms();

        OpenGLMeshData* getOrCreateMeshData(Handle mesh);
		void drawMesh(const RenderCommand& cmd);
        void uploadMeshToGPU(const Handle mesh, OpenGLMeshData* glMeshData);
        void setupVertexAttributes();
        void bindVertexArray(GLuint vao);
        void clear(float r, float g, float b, float a);

        CMap<size_t, std::unique_ptr<OpenGLMeshData>> m_meshCache;
        CMap<size_t, std::unique_ptr<OpenGLMaterialData>> m_materialCache;
        CMap<std::string, GLuint> m_shaderCache;

        // Current render state
        GLuint currentShaderProgram = 0;
        GLuint currentVAO = 0;

        // Default shader for basic rendering
        GLuint defaultShaderProgram = 0;

        std::vector<Light> m_cachedLights;
		bool m_lightsDirty = true;
	};
}

CHAI_PLUGIN_CLASS(OpenGLPlugin) 
{
	CHAI_SERVICE_AS(chai::brew::Renderer, chai::brew::OpenGLBackend, "Renderer")
}
CHAI_REGISTER_PLUGIN(OpenGLPlugin, "OpenGLRenderer", "1.0.0")