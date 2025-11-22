#pragma once
#include <string>
#include <vector>



namespace chai {

    enum class AttributeType {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        UnsignedByte,
        UnsignedByte4,
        Mat3,
        Mat4
    };

    struct VertexAttribute {
        std::string name;
        AttributeType type;
        uint32_t offset;
        bool normalized;

        static uint32_t getSize(AttributeType type) {
            switch (type) {
                case AttributeType::Float:          return sizeof(float);
                case AttributeType::Float2:         return sizeof(float) * 2;
                case AttributeType::Float3:         return sizeof(float) * 3;
                case AttributeType::Float4:         return sizeof(float) * 4;
                case AttributeType::Int:            return sizeof(int);
                case AttributeType::Int2:           return sizeof(int) * 2;
                case AttributeType::Int3:           return sizeof(int) * 3;
                case AttributeType::Int4:           return sizeof(int) * 4;
                case AttributeType::UnsignedByte:   return sizeof(uint8_t);
                case AttributeType::UnsignedByte4:  return sizeof(uint8_t) * 4;
                case AttributeType::Mat3:           return sizeof(float) * 3 * 3;
                case AttributeType::Mat4:           return sizeof(float) * 4 * 4;
                default:    return 0;
            }
        }

        uint32_t getSize() const {
            return getSize(type);
        }

        static uint32_t getComponentCount(AttributeType type) {
            switch (type) {
                case AttributeType::Float:
                case AttributeType::Int:
                case AttributeType::UnsignedByte:   return 1;
                case AttributeType::Float2:
                case AttributeType::Int2:           return 2;
                case AttributeType::Float3:
                case AttributeType::Int3:           return 3;
                case AttributeType::Float4:
                case AttributeType::Int4:
                case AttributeType::UnsignedByte4:  return 4;
                case AttributeType::Mat3:           return 9;
                case AttributeType::Mat4:           return 16;
                default:    return 0;
            }
        }

        uint32_t getComponentCount() const {
            return getComponentCount(type);
        }
    };

    class VertexLayout {
      public:
        VertexLayout() = default;

        void addAttribute(const std::string& name, AttributeType type, bool normalized = false) {
            VertexAttribute attr;
            attr.name = name;
            attr.type = type;
            attr.offset = m_stride;
            attr.normalized = normalized;

            m_attributes.push_back(attr);

            m_stride += attr.getSize();
        }

        const std::vector<VertexAttribute>& getAttributes() const {
            return m_attributes;
        }

        uint32_t getStride() const {
            return m_stride;
        }

        // Find attribute by name
        const VertexAttribute* findAttribute(const std::string& name) const {
            for (const auto& attr : m_attributes) {
                if (attr.name == name) {
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
