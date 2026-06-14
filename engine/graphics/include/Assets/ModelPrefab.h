#pragma once
#include <Handle.h>
#include <ChaiMath.h>
#include <string>

namespace chai::gfx
{
    // Tag types for Handle<>; full definitions not needed here.
    struct Mesh;
    struct Material;

    struct ModelPrefab 
    {
        /// One drawable: a geometry + the material to render it with.
        struct Primitive {
            Handle<Mesh> mesh{};
            Handle<Material> material{};
        };

        struct MeshGroup {
            std::vector<Primitive> primitives;
            std::string name;
        };

        struct Node {
            math::Mat4 local{};
            int meshGroup = -1;        // index into meshGroups, or -1 (pure transform node)
            std::vector<int> children; // indices into nodes
            std::string name;
        };

        std::vector<MeshGroup> meshGroups;
        std::vector<Node> nodes;
        std::vector<int> roots; 
    };
}