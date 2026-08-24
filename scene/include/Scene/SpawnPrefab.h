#pragma once
#include <Scene/GameObject.h>
#include <Scene/Scene.h>
#include <Assets/ModelPrefab.h>

namespace chai::scene
{
    struct SpawnParams {
        std::string name = "Prefab";
        GameObject* parent = nullptr;
    };

    struct PrefabInstance {
        GameObject* root = nullptr;
        std::vector<GameObject*> nodeObjects;
    };

    PrefabInstance
    spawn(Scene& scene, const gfx::ModelPrefab& prefab, const SpawnParams& params = {});
} // namespace chai::scene