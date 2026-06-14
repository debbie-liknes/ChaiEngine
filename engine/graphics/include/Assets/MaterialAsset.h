#pragma once
#include <ChaiMath.h>
#include <Common/GraphicsEnums.h>
#include <Common/GraphicsMacros.h>
#include <Handle.h>
#include <Assets/TextureAsset.h>

namespace chai::gfx
{
    struct Material;

    struct alignas(16) MaterialFactors {
        math::Vec4 baseColor{1.f, 1.f, 1.f, 1.f}; // offset 0
        math::Vec4 emissive{0.f, 0.f, 0.f, 0.f};  // offset 16 (xyz used)
        float metallic = 1.f;                     // offset 32
        float roughness = 1.f;                    // offset 36
        float alphaCutoff = 0.5f;                 // offset 40
        float _pad = 0.f;                         // offset 44 -> 48 total
    };
    ENFORCE_STD140_ALIGNMENT(MaterialFactors)

	struct MaterialAsset
	{
        math::Vec4 baseColorFactor;
        float metallic;
        float roughness;
        math::Vec3 emissiveFactor;
        AlphaMode alphaMode;
        bool doubleSided;

        Handle<gfx::Texture> baseColor;
        Handle<gfx::Texture> emissive;
        Handle<gfx::Texture> normal;
        Handle<gfx::Texture> metallicRoughness;
        Handle<gfx::Texture> occlusion;
	};
}