#pragma once
#include <CoreExport.h>

#include <Plugin/PluginBase.h>
#include <memory>
#include <typeindex>

namespace chai
{
static constexpr uint32_t INVALID_HANDLE = 0xFFFFFFFF;

struct CORE_EXPORT Handle
{
    uint32_t index;
    uint32_t generation;
    std::type_index type;

    Handle() : index(INVALID_HANDLE), generation(0), type(typeid(void)) {}

    Handle(uint32_t idx, uint32_t gen, std::type_index t)
        : index(idx), generation(gen), type(t) {}


    bool operator==(const Handle& other) const
    {
        return index == other.index;
    }

    bool operator!=(const Handle& other) const
    {
        return index != other.index;
    }

    bool isValid() const
    {
        return index != INVALID_HANDLE;
    }
};

struct AssetHandle : public Handle {};

struct ResourceHandle : public Handle {};
}