#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMaterial.h>
#include <Types/CMap.h>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <ChaiEngine/PipelineState.h>
#include <Resource/Resource.h>
#include <Graphics/MaterialParameter.h>

namespace chai
{
    enum class MaterialFeature 
    {
        BaseColor,
        BaseColorTexture,
        Normal,
        NormalTexture,
        Metallic,
        MetallicTexture,
        Roughness,
        RoughnessTexture,
        Emission,
        EmissionTexture,
        Transparency,
        DoubleSided
    };

    //shared by multiple instances
    struct MaterialAsset : public IAsset 
    {
    public:
		MaterialAsset() = default;
        MaterialAsset(const std::string& name, Handle shader)
            : m_name(name), m_shaderHandle(shader) 
        {}

        // Getters
        const std::string& getName() const { return m_name; }
        AssetHandle getShaderHandle() const { return m_shaderHandle; }

		bool isValid() const override { return m_valid; }
		const std::string& getAssetId() const override { return m_assetId; }

        void setParameter(const std::string& name, const MaterialParameterValue& value) {
            m_parameters[name] = value;
        }

        // Convenience overloads for specific types
        void setFloat(const std::string& name, float value) {
            m_parameters[name] = value;
        }

        void setVec3(const std::string& name, const Vec3& value) {
            m_parameters[name] = value;
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getParameters() const {
            return m_parameters;
        }

        // Get specific parameter
        const MaterialParameterValue* getParameter(const std::string& name) const {
            auto it = m_parameters.find(name);
            return it != m_parameters.end() ? &it->second : nullptr;
        }

        //this needs to go somewhere else
        struct RenderState {
            bool depthTest = true;
            bool depthWrite = true;
            bool blend = false;
        };

        const RenderState& getRenderState() const { return m_renderState; }
        void setRenderState(const RenderState& state) { m_renderState = state; }

    private:
        std::string m_name;
        AssetHandle m_shaderHandle;
        std::unordered_map<std::string, MaterialParameterValue> m_parameters;
        RenderState m_renderState;
    };

	//GPU resource representation
	//PSO etc
    struct MaterialResource : public Resource
    {
        AssetHandle sourceAsset;
        AssetHandle shaderAsset;

        MaterialLayout materialLayout;

        std::unordered_map<std::string, MaterialParameterValue> defaultParameters;

        explicit MaterialResource(AssetHandle source) : Resource(source), sourceAsset(source)
        {}
        MaterialResource() = default;
    };

	//Runtime material instance (parameters unique per instance)
    class MaterialInstance : public MaterialResource
    {
    public:
        explicit MaterialInstance(AssetHandle source)
        {}
        explicit MaterialInstance(ResourceHandle resource) : m_resourceHandle(resource) {}

        ResourceHandle getResource() const {
            return m_resourceHandle;
        }

        // Set parameter override
        void setParameter(const std::string& name, const MaterialParameterValue& value) {
            m_parameterOverrides[name] = value;
        }

        // Convenience overloads
        void setFloat(const std::string& name, float value) {
            m_parameterOverrides[name] = value;
        }

        void setVec3(const std::string& name, const Vec3& value) {
            m_parameterOverrides[name] = value;
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getParameterOverrides() const {
            return m_parameterOverrides;
        }

        bool hasOverride(const std::string& name) const {
            return m_parameterOverrides.contains(name);
        }
    private:
        ResourceHandle m_resourceHandle;
        std::unordered_map<std::string, MaterialParameterValue> m_parameterOverrides;
    };
}