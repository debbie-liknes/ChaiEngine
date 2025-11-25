#pragma once
#include <stdint.h>

namespace chai
{
    /**
     * Enum to represent generic graphics types
     */
    enum class DataType
    {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool,
        UnsignedByte,
        UnsignedByte4,
        Mat3,
        Mat4,
        Sampler2D,
        SamplerCube
    };

    /**
     *
     * @param type
     * @return The size of entire data type
     */
    inline uint32_t getDataTypeSize(DataType type)
    {
        switch (type) {
            case DataType::Float: return 4;
            case DataType::Float2: return 8;
            case DataType::Float3: return 12;
            case DataType::Float4: return 16;
            case DataType::Int: return 4;
            case DataType::Int2: return 8;
            case DataType::Int3: return 12;
            case DataType::Int4: return 16;
            case DataType::Mat3: return 36;
            case DataType::Mat4: return 64;
            case DataType::Bool: return 4;
            case DataType::UnsignedByte: return 1;
            case DataType::UnsignedByte4: return 4;
            default: return 0;
        }
    }

    /**
     *
     * @param type
     * @return Number of components inside 1 type
     */
    inline uint32_t getDataTypeComponentCount(DataType type)
    {
        switch (type) {
            case DataType::Float:
            case DataType::Int:
            case DataType::Bool:
            case DataType::UnsignedByte: return 1;
            case DataType::Float2:
            case DataType::Int2: return 2;
            case DataType::Float3:
            case DataType::Int3: return 3;
            case DataType::Float4:
            case DataType::Int4:
            case DataType::UnsignedByte4: return 4;
            case DataType::Mat3: return 9;
            case DataType::Mat4: return 16;
            default: return 0;
        }
    }

    /**
     *
     * @param type
     * @return true if the type is Sampler2D or SamplerCube
     */
    inline bool isTextureType(DataType type)
    {
        return type == DataType::Sampler2D || type == DataType::SamplerCube;
    }
}