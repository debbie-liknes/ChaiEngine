/**
 * @file ModelPrefab.h
 */
#pragma once
#include <Handle.h>
#include <ChaiMath.h>
#include <string>

namespace chai::gfx
{
    struct Mesh;
    struct Material;

    /**
     * @brief GPU ready template. Model Assets are consumed to create a prefab
     */
    struct ModelPrefab 
    {
        /**
         * @brief One drawable
         */
        struct Primitive {
            Handle<Mesh> mesh{};
            Handle<Material> material{};
        };

        /**
         * @brief Collection of primitives
         */
        struct MeshGroup {
            std::vector<Primitive> primitives;
            std::string name;
        };

        /**
         * @brief Transform data
         */
        struct Node {
            math::Vec3 position;
            math::Quat rotation;
            math::Vec3 scale;
            int meshGroup = -1;
            std::vector<int> children; // indices into nodes
            std::string name;
        };

        std::vector<MeshGroup> meshGroups;
        std::vector<Node> nodes;
        std::vector<int> roots; 
    };
}