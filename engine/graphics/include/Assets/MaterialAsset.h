/**
 * @file MaterialAsset.h
 */
#pragma once
#include <ChaiMath.h>
#include <Common/GraphicsEnums.h>
#include <Common/GraphicsMacros.h>
#include <Handle.h>
#include <Assets/TextureAsset.h>
#include <string>

namespace chai::gfx
{
    struct Material;

    /**
     * @brief Intended for use on the GPU, hence the std 140 requirement
     */
    struct alignas(16) MaterialFactors {
        math::Vec4 baseColor{1.f, 1.f, 1.f, 1.f}; // offset 0
        math::Vec4 emissive{0.f, 0.f, 0.f, 0.f};  // offset 16 (xyz used)
        float metallic = 1.f;                     // offset 32
        float roughness = 1.f;                    // offset 36
        float alphaCutoff = 0.5f;                 // offset 40
        float _pad = 0.f;                         // offset 44 -> 48 total
    };
    ENFORCE_STD140_ALIGNMENT(MaterialFactors);

    /**
     * @brief The CPU asset data
     * 
     * @note This is different from the MaterialFactors structure, because it is not
     * packed for std140. This contains nice things like handles and name data, not
     * required by GPU
     */
	struct MaterialAsset
	{
        std::string name;
        math::Vec4 baseColorFactor;
        float metallic;
        float roughness;
        math::Vec3 emissiveFactor;
        AlphaMode alphaMode;
        bool doubleSided;
        float alphaCutoff;

        Handle<gfx::Texture> baseColor;
        Handle<gfx::Texture> emissive;
        Handle<gfx::Texture> normal;
        Handle<gfx::Texture> metallicRoughness;
        Handle<gfx::Texture> occlusion;
	};
}