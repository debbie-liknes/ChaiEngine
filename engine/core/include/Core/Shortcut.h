#pragma once

#include <optional>
#include <cstddef>
#include <string>

namespace chai
{
    enum class Modifier : uint8_t
    {
        NONE    = 0,
        CTRL    = 1 << 0,
        SHIFT   = 1 << 1,
        ALT     = 1 << 2,
        SUPER   = 1 << 3 // OS-specific (Win / MacOS key)
    };

    constexpr Modifier operator|(Modifier lhs, Modifier rhs)
    {
        return static_cast<Modifier>(static_cast<std::byte>(lhs) | static_cast<std::byte>(rhs));
    }

    constexpr Modifier operator&(Modifier lhs, Modifier rhs)
    {
        return static_cast<Modifier>(static_cast<std::byte>(lhs) & static_cast<std::byte>(rhs));
    }

    constexpr Modifier operator~(Modifier val)
    {
        return static_cast<Modifier>(~static_cast<std::byte>(val));
    }

    inline Modifier& operator|=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline Modifier& operator&=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline bool hasFlag(Modifier flags, Modifier flag)
    {
        return (flags & flag) == flag;
    }

    struct Shortcut
    {
        Modifier modifier = Modifier::NONE;
        std::optional<char> key;

        std::string modifierStr() const;

        explicit operator std::string() const;

        static std::optional<Shortcut> fromString(std::string_view str);
    };
}
