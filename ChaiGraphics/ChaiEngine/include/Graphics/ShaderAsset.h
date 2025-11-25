#pragma once
#include <Asset/AssetLoader.h>
#include <Graphics/MaterialParameter.h>

namespace chai
{
    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Geometry,
        Compute
    };

    struct ShaderStageSource
    {
        ShaderStage stage;
        std::string sourceCode;
    };

    struct ShaderStageAsset : public IAsset
    {
        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const { return m_assetId; }

        ShaderStageSource source;
    };

    class ShaderAsset : public IAsset
    {
        friend class MaterialSystem;
    public:
        struct VertexInput
        {
            std::string name; // Attribute name in shader
            uint32_t location; // layout(location = X)
            DataType type; // Expected type
        };

        struct UniformDescriptor
        {
            std::string name;
            DataType type;
            bool isRequired = true;
            MaterialParameterValue defaultValue;
        };

        explicit ShaderAsset(const std::string& name) : m_name(name)
        {

        }

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const override { return m_assetId; }

        // What shader expects from materials
        const std::vector<UniformDescriptor>& getUniforms() const { return m_uniforms; }

        const std::vector<ShaderStageAsset>& getStages() const
        {
            return m_shaderStages;
        }

        // What shader expects from meshes
        const std::vector<VertexInput>& getVertexInputs() const { return m_vertexInputs; }

        void addUniform(const std::string& name, DataType type,
                       bool required = true, MaterialParameterValue defaultValue = {})
        {
            m_uniforms.push_back({name, type, required, defaultValue});
        }

        void addVertexInput(const std::string& name, uint32_t location, DataType type)
        {
            m_vertexInputs.push_back({name, location, type});
        }

        void addStage(ShaderStageAsset shaderStageAsset)
        {
            m_shaderStages.push_back(shaderStageAsset);
        }

    private:
        std::vector<UniformDescriptor> m_uniforms;
        std::vector<VertexInput> m_vertexInputs;
        std::vector<ShaderStageAsset> m_shaderStages;
        std::string m_name;
    };
}