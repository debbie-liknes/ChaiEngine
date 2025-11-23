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
    public:
        struct VertexInput
        {
            std::string name; // Attribute name in shader
            uint32_t location; // layout(location = X)
            MaterialParameterType type; // Expected type
        };

        ShaderAsset(const std::string& name) : m_name(name)
        {
        }

        bool isValid() const override { return m_valid; }
        const std::string& getAssetId() const { return m_assetId; }

        const std::string& getName() const { return m_name; }

        // Shader stages (vertex, fragment, etc.)
        void addStage(const ShaderStageSource& stage)
        {
            m_stages.push_back(stage);
        }

        const std::vector<ShaderStageSource>& getStages() const
        {
            return m_stages;
        }

        // The shader's parameter interface
        void setLayout(const MaterialLayout& layout)
        {
            m_layout = layout;
        }

        const MaterialLayout& getLayout() const
        {
            return m_layout;
        }

        // Convenience: add parameter directly
        void addParameter(const std::string& name, MaterialParameterType type, MaterialParameterValue defaultValue = {})
        {
            m_layout.addParameter(name, type, defaultValue);
        }

        void addVertexInput(const std::string& name, uint32_t location, MaterialParameterType type)
        {
            m_vertexInputs.push_back({name, location, type});
        }

        const std::vector<VertexInput>& getVertexInputs() const
        {
            return m_vertexInputs;
        }

    private:
        std::string m_name;
        std::vector<ShaderStageSource> m_stages;
        MaterialLayout m_layout; // Describes what parameters this shader expects
        std::vector<VertexInput> m_vertexInputs;
    };
}