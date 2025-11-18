#pragma once
#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>
#include <Resource/ResourceManager.h>
#include <Types/CMap.h>
#include <glad/gl.h>
#include <vector>
#include <set>
#include <sstream>
#include <memory>
#include <OpenGLRenderer/OpenGLShader.h>

namespace chai::brew
{
    // Forward declare
    class GLShaderManager;

    struct OpenGLMaterialData
    {
        GLuint shaderProgram = 0;
        bool isCompiled = false;
        bool isTransparent = false;

        // Material properties
        CMap<std::string, Handle> textures;
        CMap<std::string, std::unique_ptr<UniformBufferBase>> uniforms;

        // Cached uniform locations (per material instance)
        CMap<std::string, GLint> uniformLocations;
    };

    class OPENGLRENDERER_EXPORT OpenGLMaterialManager
    {
    public:
        OpenGLMaterialManager() = default;
        virtual ~OpenGLMaterialManager() = default;

        OpenGLMaterialManager(const OpenGLMaterialManager&) = delete;
        OpenGLMaterialManager& operator=(const OpenGLMaterialManager&) = delete;

        OpenGLMaterialManager(OpenGLMaterialManager&&) = default;
        OpenGLMaterialManager& operator=(OpenGLMaterialManager&&) = default;

        // Set the shader manager (needed for compilation)
        void setShaderManager(GLShaderManager* shaderManager)
        {
            m_shaderManager = shaderManager;
        }

        OpenGLMaterialData* getOrCreateMaterialData(Handle material)
        {
            auto it = m_materialCache.find(material.index);
            if (it == m_materialCache.end())
            {
                auto glMaterialData = std::make_unique<OpenGLMaterialData>();
                auto* ptr = glMaterialData.get();
                m_materialCache[material.index] = std::move(glMaterialData);
                return ptr;
            }
            return it->second.get();
        }

        bool compileMaterial(Handle materialHandle, OpenGLMaterialData* glMaterialData)
        {
            if (!glMaterialData || glMaterialData->isCompiled)
            {
                return glMaterialData != nullptr;
            }

            if (!m_shaderManager)
            {
                std::cerr << "Cannot compile material: shader manager not set!" << std::endl;
                return false;
            }

            std::cout << "Compiling material..." << std::endl;

            // Get or create the shared Phong shader
            GLuint phongShader = m_shaderManager->getOrCreatePhongShader();
            if (phongShader == 0)
            {
                std::cerr << "Failed to get Phong shader!" << std::endl;
                return false;
            }

            glMaterialData->shaderProgram = phongShader;

            // Set default material properties if not already set
            if (glMaterialData->uniforms.find("u_DiffuseColor") == glMaterialData->uniforms.end())
            {
                auto diffuse = std::make_unique<UniformBuffer<Vec3>>();
                diffuse->setValue(Vec3(0.8f, 0.8f, 0.8f));  // Default gray
                glMaterialData->uniforms["u_DiffuseColor"] = std::move(diffuse);
            }

            if (glMaterialData->uniforms.find("u_SpecularColor") == glMaterialData->uniforms.end())
            {
                auto specular = std::make_unique<UniformBuffer<Vec3>>();
                specular->setValue(Vec3(0.5f, 0.5f, 0.5f));  // Default gray
                glMaterialData->uniforms["u_SpecularColor"] = std::move(specular);
            }

            if (glMaterialData->uniforms.find("u_Shininess") == glMaterialData->uniforms.end())
            {
                auto shininess = std::make_unique<UniformBuffer<float>>();
                shininess->setValue(32.0f);  // Default shininess
                glMaterialData->uniforms["u_Shininess"] = std::move(shininess);
            }

            glMaterialData->isCompiled = true;
            glMaterialData->isTransparent = false;

            std::cout << "Material compiled successfully!" << std::endl;
            return true;
        }

    private:
        CMap<size_t, std::unique_ptr<OpenGLMaterialData>> m_materialCache;
        GLShaderManager* m_shaderManager = nullptr;
    };
}