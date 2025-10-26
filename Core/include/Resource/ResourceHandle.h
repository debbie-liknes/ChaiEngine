#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <optional>

namespace chai
{
    //rename this file to asset handle

    struct CORE_EXPORT Handle 
    {
        size_t index = 0;
        uint64_t generation = 0;
        std::type_index type = std::type_index(typeid(void));

        bool operator==(const Handle& other) const 
        {
            return index == other.index &&
                generation == other.generation &&
                type == other.type;
        }

        bool operator!=(const Handle& other) const 
        {
            return !(*this == other);
        }

        size_t hash() const 
        {
            size_t h1 = std::hash<size_t>()(index);
            size_t h2 = std::hash<uint64_t>()(generation);
            size_t h3 = std::hash<std::type_index>()(type);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }

        bool isValid() const 
        {
            return generation != 0 || index != 0;
        }
    };
}