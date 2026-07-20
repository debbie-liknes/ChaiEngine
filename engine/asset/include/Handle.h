/**
 * @file Handle.h
 * @brief Identifier for a live asset slot
 */

#pragma once

#include <cstdint>
#include <functional>

namespace chai
{
    /**
     * @brief A stable identifier for a live asset slot. The index is the position in the cache
     */
    template <typename T>
    struct Handle {
        using Tag = T;

        static constexpr std::uint32_t kInvalidIndex = 0xFFFF'FFFFu;

        std::uint32_t index = kInvalidIndex;
        std::uint32_t generation = 0;

        /**
         * @brief Whether this handle points to a live slot
         * @return True if the handle is valid, false if it's invalid
         */
        [[nodiscard]] constexpr bool valid() const noexcept { return index != kInvalidIndex; }
        constexpr explicit operator bool() const noexcept { return valid(); }

        friend constexpr bool operator==(Handle, Handle) noexcept = default;

        [[nodiscard]] constexpr std::uint64_t bits() const noexcept
        {
            return (static_cast<std::uint64_t>(generation) << 32) | index;
        }
    };

    template <typename T>
    inline constexpr Handle<T> kInvalidHandle{};

} // namespace chai

template <typename T>
struct std::hash<chai::Handle<T>> {
    std::size_t operator()(const chai::Handle<T>& h) const noexcept
    {
        return std::hash<std::uint64_t>{}(h.bits());
    }
};