#pragma once
#include <iostream>
#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>
#include <Resource/ResourceManager.h>
#include <Types/CMap.h>
#include <glad/gl.h>
#include <sstream>
#include <memory>
#include <OpenGLRenderer/OpenGLShader.h>

#include "Graphics/ShaderAsset.h"

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

        bool compileMaterial(ResourceHandle materialHandle, OpenGLMaterialData* glMaterialData)
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

            //Prefer to use a material instance, but fallback to a material resource
            const MaterialResource* matResource;
            const auto* materialInstance = ResourceManager::instance().getResource<MaterialInstance>(
                materialHandle);
            if (materialInstance)
            {
                matResource = ResourceManager::instance().getResource<MaterialResource>(
                    materialInstance->getResource());
            }
            else
            {
                matResource = ResourceManager::instance().getResource<MaterialResource>(materialHandle);
            }

            if (!matResource)
            {
                std::cerr << "Failed to get material resource!" << std::endl;
                return false;
            }

            AssetHandle shaderAssetHandle = matResource->shaderAsset;
            const auto* shaderAsset = AssetManager::instance().get<ShaderAsset>(shaderAssetHandle);
            if (!shaderAsset)
            {
                std::cerr << "Failed to get shader asset for material!" << std::endl;
                return false;
            }

            std::cout << "Compiling material with shader: " << shaderAsset->getName() << std::endl;

            GLuint shaderProgram = m_shaderManager->compileShaderFromAsset(shaderAssetHandle);
            if (shaderProgram == 0)
            {
                std::cerr << "Failed to compile shader for material!" << std::endl;
                return false;
            }

            glMaterialData->shaderProgram = shaderProgram;

            const auto& materialParams = matResource->defaultParameters;

            for (const auto& [paramName, paramValue] : materialParams)
            {
                GLint location = glGetUniformLocation(shaderProgram, paramName.c_str());
                if (location != -1)
                {
                    glMaterialData->uniformLocations[paramName] = location;
                    if (materialInstance && materialInstance->hasOverride(paramName))
                    {
                        auto& overrides = materialInstance->getParameterOverrides();
                        glMaterialData->uniforms[paramName] = createUniformBuffer(
                            overrides.at(paramName));
                    }
                    else
                    {
                        glMaterialData->uniforms[paramName] = createUniformBuffer(paramValue);
                    }
                }
            }

            glMaterialData->isCompiled = true;
            std::cout << "Created material shader: " << shaderAsset->getName() << std::endl;
            return true;
        }

    private:
        std::unique_ptr<UniformBufferBase> createUniformBuffer(const MaterialParameterValue& value)
        {
            return std::visit([](auto&& arg) -> std::unique_ptr<UniformBufferBase>
                              {
                                  using T = std::decay_t<decltype(arg)>;

                                  if constexpr (std::is_same_v<T, float>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<float>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Vec3>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Vec3>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Vec4>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Vec4>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, int>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<int>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Mat4>)
                                  {
                                      auto buf = std::make_unique<UniformBuffer<Mat4>>();
                                      buf->setValue(arg);
                                      return buf;
                                  }
                                  else if constexpr (std::is_same_v<T, Handle>) // Texture handle
                                  {
                                      // Textures are handled separately
                                      return nullptr;
                                  }
                                  else
                                  {
                                      return nullptr;
                                  }
                              },
                              value);
        }

        CMap<size_t, std::unique_ptr<OpenGLMaterialData>> m_materialCache;
        GLShaderManager* m_shaderManager = nullptr;
    };
}