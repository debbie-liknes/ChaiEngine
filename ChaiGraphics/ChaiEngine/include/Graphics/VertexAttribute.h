#pragma once
#include <Graphics/GraphicsTypes.h>
#include <string>
#include <vector>


namespace chai
{
    struct VertexAttribute
    {
        std::string name;
        DataType type;
        uint32_t offset;
        bool normalized;

        uint32_t getSize() const
        {
            return getDataTypeSize(type);
        }

        uint32_t getComponentCount() const
        {
            return getDataTypeComponentCount(type);
        }
    };

    class VertexLayout
    {
    public:
        VertexLayout() = default;

        void addAttribute(const std::string& name, DataType type, bool normalized = false)
        {
            VertexAttribute attr;
            attr.name = name;
            attr.type = type;
            attr.offset = m_stride;
            attr.normalized = normalized;

            m_attributes.push_back(attr);

            m_stride += attr.getSize();
        }

        const std::vector<VertexAttribute>& getAttributes() const
        {
            return m_attributes;
        }

        uint32_t getStride() const
        {
            return m_stride;
        }

        // Find attribute by name
        const VertexAttribute* findAttribute(const std::string& name) const
        {
            for (const auto& attr : m_attributes)
            {
                if (attr.name == name)
                {
                    return &attr;
                }
            }
            return nullptr;
        }

    private:
        std::vector<VertexAttribute> m_attributes;
        uint32_t m_stride = 0;
    };
}