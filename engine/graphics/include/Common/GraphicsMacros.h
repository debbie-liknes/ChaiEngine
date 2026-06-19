#pragma once
#include <stdint.h>

    // bitwise op helper for enums
#define CHAI_FLAG_OPS(Enum)                                                                        \
    constexpr Enum operator|(Enum a, Enum b) noexcept                                              \
    {                                                                                              \
        return static_cast<Enum>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));   \
    }                                                                                              \
    constexpr Enum operator&(Enum a, Enum b) noexcept                                              \
    {                                                                                              \
        return static_cast<Enum>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));   \
    }                                                                                              \
    constexpr Enum& operator|=(Enum& a, Enum b) noexcept                                           \
    {                                                                                              \
        a = a | b;                                                                                 \
        return a;                                                                                  \
    }                                                                                              \
    constexpr bool hasFlag(Enum value, Enum flag) noexcept                                         \
    {                                                                                              \
        return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) != 0;        \
    }

#define ENFORCE_STD140_ALIGNMENT(Type)                                                             \
        static_assert(alignof(Type) == 16);                                                        \
        static_assert(sizeof(Type) % 16 == 0)
