#pragma once
#include "MeshAsset.h"
#include <string>
#include <Assets/TextureAsset.h>
#include <Common/GraphicsEnums.h>

namespace chai::gfx
{
    struct ModelAsset 
    {
        struct Primitive {
            MeshAsset mesh;
            int material = -1;
        };
        struct MeshEntry {
            std::vector<Primitive> primitives;
            std::string name;
        };

        struct MaterialDesc {
            math::Vec4 baseColorFactor{1, 1, 1, 1};
            float metallic = 1.f, roughness = 1.f;
            math::Vec3 emissiveFactor{0, 0, 0};
            int baseColor = -1, metallicRoughness = -1, normal = -1, occlusion = -1,
                emissive = -1;
            AlphaMode alphaMode = AlphaMode::Opaque;
            bool doubleSided = false;
        };

        struct Node {
            math::Mat4 local{};
            int meshIndex = -1;
            std::vector<int> children;
            std::string name;
        };

        struct ImageData {
            std::vector<uint8_t> bytes;
        };

        std::vector<MeshEntry> meshes;
        std::vector<MaterialDesc> materials;
        std::vector<ImageData> images;
        std::vector<Node> nodes;
        std::vector<int> roots;
    };
}