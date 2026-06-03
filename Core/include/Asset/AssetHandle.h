#pragma once
#include <CoreExport.h>
#include <Plugin/PluginBase.h>
#include <memory>
#include <typeindex>

namespace chai
{
    static constexpr uint32_t INVALID_HANDLE = 0xFFFFFFFF;

    template <typename T>
    struct Handle {
        uint32_t index = INVALID_HANDLE;
        uint32_t generation = 0;

        bool operator==(const Handle&) const = default;
        bool operator!=(const Handle&) const = default;
        bool isNull() const { return index == INVALID_HANDLE; }
    };
} // namespace chai