#pragma once
#include <ChaiEngine/Material.h>
#include <ChaiEngine/UniformBuffer.h>
#include <Types/CMap.h>
#include <glad/gl.h>
#include <vector>
#include <set>
#include <sstream>

namespace chai::brew
{
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

    private:
        CMap<size_t, std::unique_ptr<OpenGLMaterialData>> m_materialCache;
    };
}