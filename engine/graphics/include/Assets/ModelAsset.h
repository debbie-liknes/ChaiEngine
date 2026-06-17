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
            std::string name;
            math::Vec4 baseColorFactor{1, 1, 1, 1};
            float metallic = 0.f, roughness = 0.f;
            math::Vec3 emissiveFactor{0, 0, 0};
            int baseColor = -1, metallicRoughness = -1, normal = -1, occlusion = -1,
                emissive = -1;
            AlphaMode alphaMode = AlphaMode::Opaque;
            bool doubleSided = false;
            float alphaCutoff = 1.f;
        };

        struct Node {
            math::Vec3 position;
            math::Quat rotation{0.f, 0.f, 0.f, 1.f};
            math::Vec3 scale;
            int meshIndex = -1;
            std::vector<int> children;
            std::string name;
        };

        struct ImageData {
            std::vector<uint8_t> bytes;
            std::string name;
            //should this be an enum?
            std::string mimeType;
        };

        std::vector<MeshEntry> meshes;
        std::vector<MaterialDesc> materials;
        std::vector<ImageData> images;
        std::vector<Node> nodes;
        std::vector<int> roots;
    };
}