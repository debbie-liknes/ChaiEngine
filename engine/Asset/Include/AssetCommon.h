/**
 * @file AssetCommon.h
 * @brief Strcutures and helpers common to all asset domains
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace chai
{
    /**
     * @brief Where a slot is in its load lifecycle. 
     */
    enum class LoadState : std::uint8_t {
        Empty,      //Reserve for invalid handle
        Loading,    //acquired slot, but no cpu bytes yet
        Queued,     //cpu bytes ingested, queued for upload
        Uploading,  //waiting on createResource
        Ready,      //ready to use
        Failed      //something went wrong
    };

    /**
     * @brief A unique identifier for an asset, used for de-duplication
     */
    struct AssetId {
        std::uint64_t value = 0;

        friend constexpr bool operator==(AssetId, AssetId) noexcept = default;
        [[nodiscard]] constexpr bool valid() const noexcept { return value != 0; }
    };

    /**
     * @brief Convenience method to create an AssetId
     * @param s A string to hash into an AssetId
     * @todo this uses a simple FNV-1a hash, which is not very strong
     * @return An AssetIdderived from the input string
     */
    [[nodiscard]] constexpr AssetId makeAssetId(std::string_view s) noexcept
    {
        std::uint64_t h = 1469598103934665603ull; // offset
        for (char c : s) {
            h ^= static_cast<std::uint8_t>(c);
            h *= 1099511628211ull; // FNV prime
        }
        if (h == 0)
            h = 1;
        return AssetId{h};
    }

    template <typename T>
    struct AssetTraits;

} // namespace chai

/**
 * @brief Hash specialization for AssetId
 */
template <>
struct std::hash<chai::AssetId> {
    std::size_t operator()(const chai::AssetId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value);
    }
};