#pragma once

#include <unordered_map>
#include <string_view>

namespace chai
{
    struct StringHash {
        using is_transparent = void; // Enables heterogeneous operations.
        std::size_t operator()(std::string_view sv) const
        {
            std::hash<std::string_view> hasher;
            return hasher(sv);
        }
    };

    /*
    * Temporarily fixes cpp:S6045 static analysis warnings arround the default
    * string hashing function with transparent equality.
    *
    * \note This implementation is not permanent and should be replaced with something more robust.
    */
    template<typename V>
    using Dictionary = std::unordered_map<std::string, V, StringHash, std::equal_to<>>;
}