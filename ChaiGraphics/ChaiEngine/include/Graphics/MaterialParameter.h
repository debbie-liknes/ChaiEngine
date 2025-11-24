#pragma once
#include <ChaiMath.h>
#include <variant>
#include <vector>

namespace chai
{
    enum class MaterialParameterType
    {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Mat3,
        Mat4,
        Bool,

        //eventually
        //Sampler2D,
        //SamplerCube
    };

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
        MaterialParameterType type;
        MaterialParameterValue defaultValue;

        static uint32_t getSize(MaterialParameterType type)
        {
            switch (type)
            {
            case MaterialParameterType::Float: return 4;
            case MaterialParameterType::Float2: return 8;
            case MaterialParameterType::Float3: return 12;
            case MaterialParameterType::Float4: return 16;
            case MaterialParameterType::Int: return 4;
            case MaterialParameterType::Mat3: return 36;
            case MaterialParameterType::Mat4: return 64;
            case MaterialParameterType::Bool: return 4;
            default: return 0;
            }
        }

        uint32_t getSize() const
        {
            return getSize(type);
        }

        bool isSampler() const
        {
            return false;
        }
    };

    class MaterialLayout
    {
    public:
        MaterialLayout() = default;

        void addParameter(const std::string& name, MaterialParameterType type, MaterialParameterValue defaultValue = {})
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