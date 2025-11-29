#pragma once
#include <ChaiMath.h>
#include <variant>
#include <vector>
#include <Graphics/GraphicsTypes.h>
#include <Asset/AssetHandle.h>

namespace chai
{
    using MaterialParameterValue = std::variant<
        float,
        Vec2,
        Vec3,
        Vec4,
        int,
        Mat3,
        Mat4,
        ResourceHandle,
        bool>;

    struct MaterialParameter
    {
        std::string name;
        DataType type;
        MaterialParameterValue defaultValue;

        uint32_t getSize() const
        {
            return getDataTypeSize(type);
        }

        bool isSampler() const
        {
            return isTextureType(type);
        }
    };

    class MaterialLayout
    {
    public:
        MaterialLayout() = default;

        void addParameter(const std::string& name, DataType type, MaterialParameterValue defaultValue = {})
        {
            MaterialParameter param;
            param.name = name;
            param.type = type;
            param.defaultValue = defaultValue;

            m_parameters.push_back(param);
        }

        const std::vector<MaterialParameter>& getParameters() const
        {
            return m_parameters;
        }

        const MaterialParameter* findParameter(const std::string& name) const
        {
            for (const auto& param : m_parameters)
            {
                if (param.name == name)
                {
                    return &param;
                }
            }
            return nullptr;
        }

        std::vector<const MaterialParameter*> getUniforms() const
        {
            std::vector<const MaterialParameter*> uniforms;
            for (const auto& param : m_parameters)
            {
                if (!param.isSampler())
                {
                    uniforms.push_back(&param);
                }
            }
            return uniforms;
        }

    private:
        std::vector<MaterialParameter> m_parameters;
    };
}