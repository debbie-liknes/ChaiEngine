#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/IMaterial.h>
#include <Types/CMap.h>
#include <set>
#include <ChaiMath.h>

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

    class Material : public IMaterial
    {
    public:
        explicit Material(std::shared_ptr<ShaderDescription> shaderDesc)
            : m_shaderDescription(shaderDesc)
        {
        }

        // Feature-based material properties
        void setFeature(MaterialFeature feature, bool enabled) {
            if (enabled) 
            {
                m_enabledFeatures.insert(feature);
            }
            else 
            {
                m_enabledFeatures.erase(feature);
            }
            m_dirty = true;
        }

        bool hasFeature(MaterialFeature feature) const 
        {
            return m_enabledFeatures.find(feature) != m_enabledFeatures.end();
        }

        // Unified property setters
        void setProperty(const std::string& name, const Vec3& value) 
        {
            auto uniform = std::make_shared<UniformBuffer<Vec3>>(value);
            m_uniforms[name] = uniform;
            m_dirty = true;
        }

        void setProperty(const std::string& name, float value) 
        {
            auto uniform = std::make_shared<UniformBuffer<float>>(value);
            m_uniforms[name] = uniform;
            m_dirty = true;
        }

        void setTexture(const std::string& samplerName, uint32_t textureId, int slot = 0) override 
        {
            m_textures[samplerName] = { textureId, slot };
            m_dirty = true;
        }

        // PBR-specific setters
        void setBaseColor(const Vec3& color) 
        {
            setProperty("u_baseColor", color);
            setFeature(MaterialFeature::BaseColor, true);
        }

        void setBaseColorTexture(uint32_t textureId) 
        {
            setTexture("u_baseColorTexture", textureId, 0);
            setFeature(MaterialFeature::BaseColorTexture, true);
        }

        void setMetallic(float metallic) 
        {
            setProperty("u_metallic", metallic);
            setFeature(MaterialFeature::Metallic, true);
        }

        void setRoughness(float roughness) 
        {
            setProperty("u_roughness", roughness);
            setFeature(MaterialFeature::Roughness, true);
        }

        void setNormal(const Vec3& normal) 
        {
            setProperty("u_normal", normal);
            setFeature(MaterialFeature::Normal, true);
        }

        void setNormalTexture(uint32_t textureId) 
        {
            setTexture("u_normalTexture", textureId, 1);
            setFeature(MaterialFeature::NormalTexture, true);
        }

        void setEmission(const Vec3& emission) 
        {
            setProperty("u_emission", emission);
            setFeature(MaterialFeature::Emission, true);
        }

        void setTransparency(float alpha) 
        {
            setProperty("u_transparency", alpha);
            setFeature(MaterialFeature::Transparency, alpha < 1.0f);
        }

        void setDoubleSided(bool doubleSided) 
        {
            setFeature(MaterialFeature::DoubleSided, doubleSided);
        }

        // Legacy Phong support (for OBJ compatibility)
        void setDiffuse(const Vec3& diffuse) 
        {
            setProperty("u_diffuse", diffuse);
        }

        void setSpecular(const Vec3& specular) 
        {
            setProperty("u_specular", specular);
        }

        void setAmbient(const Vec3& ambient) 
        {
            setProperty("u_ambient", ambient);
        }

        void setShininess(float shininess) 
        {
            setProperty("u_shininess", shininess);
        }

        // Getters
        std::shared_ptr<ShaderDescription> getShaderDescription() const 
        {
            return m_shaderDescription;
        }

        std::string getShaderHash() const 
        {
            return generateHash();
        }

        const CMap<std::string, std::shared_ptr<UniformBufferBase>>& getUniforms() const 
        {
            return m_uniforms;
        }

        const CMap<std::string, TextureSlot>& getTextures() const
        {
            return m_textures;
        }

        const std::set<MaterialFeature>& getEnabledFeatures() const {
            return m_enabledFeatures;
        }

        bool isTransparent() const override 
        {
            return hasFeature(MaterialFeature::Transparency);
        }

        bool isDirty() const { return m_dirty; }
        void markClean() { m_dirty = false; }

    private:
        std::shared_ptr<ShaderDescription> m_shaderDescription;
        CMap<std::string, TextureSlot> m_textures;
        CMap<std::string, std::shared_ptr<UniformBufferBase>> m_uniforms;
        std::set<MaterialFeature> m_enabledFeatures;
        mutable bool m_dirty = true;

        std::string generateHash() const 
        {
            std::string hash = m_shaderDescription->name;
            for (const auto& stage : m_shaderDescription->stages) 
            {
                hash += stage.entryPoint;
            }
            // Include enabled features in hash for shader variant generation
            for (auto feature : m_enabledFeatures) 
            {
                hash += std::to_string(static_cast<int>(feature));
            }
            return std::to_string(std::hash<std::string>()(hash));
        }
    };

    class MaterialSystem
    {
    public:
        // PBR materials
        static std::shared_ptr<Material> createPBR() 
        {
            auto shaderDesc = std::make_shared<ShaderDescription>();
            shaderDesc->name = "pbr_standard";
            shaderDesc->stages = {
                {ShaderStage::Vertex, "shaders/pbr.vert"},
                {ShaderStage::Fragment, "shaders/pbr.frag"}
            };

            auto material = std::make_shared<Material>(shaderDesc);

            // Set default PBR values
            material->setBaseColor(Vec3(0.8f, 0.8f, 0.8f));
            material->setMetallic(0.0f);
            material->setRoughness(0.8f);

            return material;
        }

        // Legacy Phong for OBJ compatibility
        static std::shared_ptr<Material> createPhong() 
        {
            auto shaderDesc = std::make_shared<ShaderDescription>();
            shaderDesc->name = "phong_standard";
            shaderDesc->stages = {
                {.type = ShaderStage::Vertex, .path = "shaders/phong.vert"},
                {.type = ShaderStage::Fragment, .path = "shaders/phong.frag"}
            };

            auto material = std::make_shared<Material>(shaderDesc);

            // Set default Phong values
            material->setDiffuse(Vec3(0.8f, 0.8f, 0.8f));
            material->setSpecular(Vec3(0.2f, 0.2f, 0.2f));
            material->setAmbient(Vec3(0.1f, 0.1f, 0.1f));
            material->setShininess(32.0f);

            return material;
        }

        // Factory method that creates appropriate material based on features
        static std::shared_ptr<Material> createFromFeatures(const std::set<MaterialFeature>& features) 
        {
            bool needsPBR = features.count(MaterialFeature::Metallic) ||
                features.count(MaterialFeature::MetallicTexture) ||
                features.count(MaterialFeature::Roughness) ||
                features.count(MaterialFeature::RoughnessTexture);

            auto material = needsPBR ? createPBR() : createPhong();

            for (auto feature : features) {
                material->setFeature(feature, true);
            }

            return material;
        }

        static std::shared_ptr<Material> createCustom(const std::string& vertexPath,
            const std::string& fragmentPath) 
        {
            auto shaderDesc = std::make_shared<ShaderDescription>();
            shaderDesc->name = "custom_" + std::to_string(generateUniqueId());
            shaderDesc->stages = {
                {ShaderStage::Vertex, vertexPath},
                {ShaderStage::Fragment, fragmentPath}
            };
            return std::make_shared<Material>(shaderDesc);
        }

    private:
        static int generateUniqueId() 
        {
            static int idCounter = 0;
            return idCounter++;
        }
    };
}