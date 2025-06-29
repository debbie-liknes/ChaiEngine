#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace chai
{
    template <typename Key>
    struct TransparentHash 
    {
        using is_transparent = void;

        size_t operator()(const Key& key) const noexcept 
        {
            return std::hash<Key>{}(key);
        }
    };

    // Specialization for string-like keys
    template <>
    struct TransparentHash<std::string> 
    {
        using is_transparent = void;

        template <typename T>
        size_t operator()(const T& str) const noexcept 
        {
            return std::hash<std::string_view>{}(std::string_view(str));
        }
    };

    template <typename Key>
    using TransparentEqual = std::equal_to<>;

    template<
        typename Key,
        typename Value,
        typename Hash = TransparentHash<Key>,
        typename Equal = TransparentEqual<Key>
    >
    using CMap = std::unordered_map<Key, Value, Hash, Equal>;
}