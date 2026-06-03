#pragma once
#include <ChaiGraphicsExport.h>
#include <Types/CMap.h>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <Graphics/MaterialParameter.h>
#include <Graphics/ShaderAsset.h>

namespace chai
{
    //shared by multiple instances
    struct CHAIGRAPHICS_EXPORT MaterialAsset
    {
    public:
        MaterialAsset() = default;

        MaterialAsset(const std::string& name)
            : m_name(name) {}

        MaterialAsset(const std::string& name, Handle<ShaderAsset> shader)
            : m_name(name), m_shaderHandle(shader) {}

        // Getters
        const std::string& getName() const { return m_name; }
        Handle<ShaderAsset> getShaderHandle() const { return m_shaderHandle; }

        //void setParameter(const std::string& name, const MaterialParameterValue& value)
        //{
        //    m_parameters[name] = value;
        //}

        // Convenience overloads for specific types
        void setFloat(const std::string& name, float value)
        {
            m_parameters[name] = value;
        }

        void setVec3(const std::string& name, const Vec3& value)
        {
            m_parameters[name] = value;
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getParameters() const
        {
            return m_parameters;
        }

        // Get specific parameter
        const MaterialParameterValue* getParameter(const std::string& name) const
        {
            auto it = m_parameters.find(name);
            return it != m_parameters.end() ? &it->second : nullptr;
        }

        //this needs to go somewhere else
        struct RenderState
        {
            bool depthTest = true;
            bool depthWrite = true;
            bool blend = false;
        };

        const RenderState& getRenderState() const { return m_renderState; }
        void setRenderState(const RenderState& state) { m_renderState = state; }

    private:
        std::string m_name;
        Handle<ShaderAsset> m_shaderHandle{};
        std::unordered_map<std::string, MaterialParameterValue> m_parameters;
        RenderState m_renderState;
    };

    //GPU resource representation
    struct CHAIGRAPHICS_EXPORT MaterialResource
    {
        Handle<MaterialAsset> sourceAsset{};
        Handle<ShaderAsset> shaderAsset{};

        // Separate textures from scalar uniforms
        std::unordered_map<std::string, MaterialParameterValue> uniforms;

        struct TextureBinding {
            Handle<TextureResource> texture;
            int slot;
        };
        std::unordered_map<std::string, TextureBinding> textures;

        explicit MaterialResource(Handle<MaterialAsset> source) : sourceAsset(source) {}
        MaterialResource() = default;
    };

    //Runtime material instance (parameters unique per instance)
    class CHAIGRAPHICS_EXPORT MaterialInstance
    {
    public:
        explicit MaterialInstance(Handle<MaterialAsset> source) {}
        explicit MaterialInstance(Handle<MaterialResource> resource) : m_resourceHandle(resource) {}

        Handle<MaterialResource> getResource() const
        {
            return m_resourceHandle;
        }

        // Set parameter override
        void setParameter(const std::string& name, const MaterialParameterValue& value)
        {
            //m_parameterOverrides[name] = value;
        }

        // Convenience overloads
        void setFloat(const std::string& name, float value)
        {
            m_parameterOverrides[name] = value;
        }

        void setVec3(const std::string& name, const Vec3& value)
        {
            m_parameterOverrides[name] = value;
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getParameterOverrides() const
        {
            return m_parameterOverrides;
        }

        bool hasOverride(const std::string& name) const
        {
            return m_parameterOverrides.contains(name);
        }

    private:
        Handle<MaterialResource> m_resourceHandle;
        std::unordered_map<std::string, MaterialParameterValue> m_parameterOverrides;
    };
}
