#pragma once
#include <Asset/AssetHandle.h>
#include <ChaiMath.h>

namespace chai::cup
{
    struct SceneNode 
    {
        std::string name;
        Mat4 localTransform = Mat4(1.0f);
        Mat4 worldTransform = Mat4(1.0f);

        AssetHandle mesh;     // optional, not all nodes have geometry
        AssetHandle material; // override, or use mesh's default

        SceneNode* parent = nullptr;
        std::vector<std::unique_ptr<SceneNode>> children;

        void updateWorldTransform()
        {
            if (parent) {
                worldTransform = parent->worldTransform * localTransform;
            } else {
                worldTransform = localTransform;
            }

            for (auto& child : children) {
                child->updateWorldTransform();
            }
        }
    };
} // namespace chai::cup