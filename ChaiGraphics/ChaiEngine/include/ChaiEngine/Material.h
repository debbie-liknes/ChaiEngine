#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMaterial.h>
#include <Types/CMap.h>
#include <set>
#include <ChaiMath.h>
#include <Asset/AssetLoader.h>
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

    //shared by multiple instances
    struct MaterialAsset : public IAsset 
    {
        std::string shaderName;

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const override { return m_assetId; }

        // Default properties (from file)
        struct Properties {
            Vec3 diffuseColor = Vec3(0.8f, 0.8f, 0.8f);
            Vec3 specularColor = Vec3(0.2f, 0.2f, 0.2f);
            Vec3 ambientColor = Vec3(0.1f, 0.1f, 0.1f);
            float shininess = 32.0f;
        } properties;

        // Texture paths (not handles!)
        struct TexturePaths {
            std::string diffuseMap;
            std::string specularMap;
            std::string normalMap;
        } texturePaths;


        PipelineState pso;
    };

	//GPU resource representation
	//PSO etc
    struct MaterialResource : public Resource
    {
        std::string shaderName;

        MaterialResource(Handle assetHandle)
            : Resource(assetHandle) 
        {
		}

        // Properties (serialized to disk)
        std::unordered_map<std::string, Vec3> vec3Properties;
        std::unordered_map<std::string, float> floatProperties;

        // Texture paths (not handles!)
        std::unordered_map<std::string, std::string> texturePaths;
    };

	//Runtime material instance (parameters unique per instance)
    class MaterialInstance : public Resource
    {
    public:
        MaterialInstance(Handle assetHandle)
            : Resource(assetHandle),
              m_diffuseColor(0.8f, 0.8f, 0.8f),
              m_specularColor(0.2f, 0.2f, 0.2f),
              m_ambientColor(0.1f, 0.1f, 0.1f),
              m_shininess(32.0f)
        {
		}

        void setDiffuseColor(const Vec3& color) { m_diffuseColor = color; }
        void setSpecularColor(const Vec3& color) { m_specularColor = color; }
        void setAmbientColor(const Vec3& color) { m_ambientColor = color; }
        void setShininess(float shininess) { m_shininess = shininess; }

		const Vec3& getDiffuseColor() const { return m_diffuseColor; }
		const Vec3& getSpecularColor() const { return m_specularColor; }
		const Vec3& getAmbientColor() const { return m_ambientColor; }
		float getShininess() const { return m_shininess; }

    private:
        Vec3 m_diffuseColor;
        Vec3 m_specularColor;
        Vec3 m_ambientColor;
        float m_shininess;
    };

  //  class Material : public IMaterial, public IAsset
  //  {
  //  public:
  //      void setShader(const std::string& shaderName) 
  //      {
  //          m_shaderName = shaderName;
  //          m_dirty = true;
  //      }

		////Mat parameters
  //      void setProperty(const std::string& name, const Vec3& value);
  //      void setTexture(const std::string& name, Handle textureHandle);

  //      // Get compiled GPU resources (lazy, cached)
  //      Handle getShaderHandle(const std::string& passName) const;
  //      Handle getPipelineHandle(const std::string& passName, const RenderPassInfo& rpInfo) const;

  //  private:
  //      std::string m_name;
  //      std::string m_shaderName;  // "pbr_standard", NOT the actual shader!

  //      // Material instance data (this is what makes each material unique)
  //      CMap<std::string, UniformValue> m_properties;
  //      CMap<std::string, Handle> m_textureHandles;
  //      std::set<MaterialFeature> m_enabledFeatures;

  //      // Render state overrides (optional)
  //      std::optional<BlendState> m_blendOverride;
  //      std::optional<RasterizerState> m_rasterizerOverride;

  //      mutable bool m_dirty = true;
  //  };
}