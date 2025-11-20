#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMaterial.h>
#include <Types/CMap.h>
#include <set>
#include <ChaiMath.h>
#include <Asset/AssetLoader.h>
#include <Asset/AssetHandle.h>
#include <Asset/AssetManager.h>
#include <ChaiEngine/PipelineState.h>
#include <Resource/Resource.h>

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

    struct MaterialPass
    {
        std::string name;  // "main", "shadow", "depth_prepass", "outline"
        PipelineDesc pipeline;

        std::unordered_map<std::string, UniformBufferBase> uniformOverrides;

        bool enabled = true;
    };

    using MaterialParameterValue = std::variant<
        float,
        Vec2,
        Vec3,
        Vec4,
        //Handle<TextureAsset>, // Textures dont work yet
        int,
        bool
    > ;

    struct MaterialParameter {
        std::string name;
        MaterialParameterValue defaultValue;
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
        Handle getShaderHandle() const { return m_shaderHandle; }

		bool isValid() const override { return m_valid; }
		const std::string& getAssetId() const override { return m_assetId; }

        const std::unordered_map<std::string, MaterialParameter>& getParameters() const 
        {
            return m_parameters;
        }

        // Get a specific parameter by name
        const MaterialParameter* getParameter(const std::string& name) const 
        {
            auto it = m_parameters.find(name);
            return it != m_parameters.end() ? &it->second : nullptr;
        }

        // Add parameter (typically called during asset loading)
        void addParameter(const std::string& name, MaterialParameterValue defaultValue) 
        {
            m_parameters[name] = MaterialParameter{ name, defaultValue };
        }

        // Optional: render state hints
        struct RenderState 
        {
            bool depthTest = true;
            bool depthWrite = true;
            bool blend = false;
            // blending modes, culling, etc.
        };

        const RenderState& getRenderState() const { return m_renderState; }
        void setRenderState(const RenderState& state) { m_renderState = state; }

    private:
        std::string m_name;
        Handle m_shaderHandle;
        std::unordered_map<std::string, MaterialParameter> m_parameters;
        RenderState m_renderState;
    };

	//GPU resource representation
	//PSO etc
    class MaterialResource : public Resource
    {
    public:
		MaterialResource() = default;

        explicit MaterialResource(Handle assetHandle) : Resource(assetHandle), 
			m_assetHandle(assetHandle), m_platformMaterial(nullptr)
        {
        }

        ~MaterialResource() = default;

        // Get the source asset
        Handle getAssetHandle() const { return m_assetHandle; }

        // Get platform-specific material (cast to OpenGLMaterial* etc.)
        void* getPlatformMaterial() const { return m_platformMaterial; }

        template<typename T>
        T* getPlatformMaterial() const 
        {
            return static_cast<T*>(m_platformMaterial);
        }

        // Runtime parameter overrides (optional - if you want to modify at runtime)
        void setParameterOverride(const std::string& name, MaterialParameterValue value) 
        {
            m_parameterOverrides[name] = value;
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getParameterOverrides() const 
        {
            return m_parameterOverrides;
        }

        bool hasParameterOverride(const std::string& name) const 
        {
            return m_parameterOverrides.find(name) != m_parameterOverrides.end();
        }

    private:
        Handle m_assetHandle;
        void* m_platformMaterial; // Opaque pointer to OpenGLMaterial, etc.

        std::unordered_map<std::string, MaterialParameterValue> m_parameterOverrides;
    };

	//Runtime material instance (parameters unique per instance)
    class MaterialInstance
    {
    public:
        explicit MaterialInstance(Handle materialResource): m_materialResource(materialResource) 
        {}

        // Get the base material resource
        Handle getMaterialResource() const 
        {
            return m_materialResource;
        }

        // Set per-instance parameters
        void setParameter(const std::string& name, MaterialParameterValue value) 
        {
            m_instanceParameters[name] = value;
        }

        // Get parameter value (checks instance overrides first, then material resource, then asset)
        template<typename T>
        T getParameter(const std::string& name, const MaterialAsset* materialAsset,
            const MaterialResource* materialResource) const 
        {
            // 1. Check instance override
            auto it = m_instanceParameters.find(name);
            if (it != m_instanceParameters.end()) 
            {
                return std::get<T>(it->second);
            }

            // 2. Check material resource override (if you support runtime material modification)
            if (materialResource && materialResource->hasParameterOverride(name)) 
            {
                return std::get<T>(materialResource->getParameterOverrides().at(name));
            }

            // 3. Fall back to asset default
            if (materialAsset) 
            {
                const auto* param = materialAsset->getParameter(name);
                if (param) 
                {
                    return std::get<T>(param->defaultValue);
                }
            }

            // Return default-constructed value if not found
            return T{};
        }

        const std::unordered_map<std::string, MaterialParameterValue>& getInstanceParameters() const 
        {
            return m_instanceParameters;
        }

        bool hasInstanceParameter(const std::string& name) const 
        {
            return m_instanceParameters.find(name) != m_instanceParameters.end();
        }

        void clearParameter(const std::string& name) 
        {
            m_instanceParameters.erase(name);
        }

    private:
        Handle m_materialResource;
        std::unordered_map<std::string, MaterialParameterValue> m_instanceParameters;
    };
}