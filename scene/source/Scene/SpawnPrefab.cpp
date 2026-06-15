#include <Scene/SpawnPrefab.h>
#include <Components/TransformComponent.h>
#include <Components/MeshComponent.h>

namespace chai::scene
{
    PrefabInstance spawn(Scene& scene, const gfx::ModelPrefab& prefab, const SpawnParams& params)
    {
        std::vector<GameObject*> created(prefab.nodes.size(), nullptr);
        PrefabInstance instance;

        //gotta be recursive
        auto spawn = [&](auto&& self, int nodeIdx, GameObject* parentObj) -> void {
            const gfx::ModelPrefab::Node& node = prefab.nodes[nodeIdx];

            GameObject* obj = scene.createObject(node.name.empty() ? "node" : node.name.c_str());
            if (!instance.root)
                instance.root = obj;

            auto* tf = obj->getComponent<TransformComponent>();
            tf->setPosition(node.position);
            tf->setRotation(node.rotation);
            tf->setScale(node.scale);
            //tf->setLocalMatrix(node.local);

            if (parentObj)
                obj->setParent(parentObj);

            if (node.meshGroup >= 0) {
                const gfx::ModelPrefab::MeshGroup& group = prefab.meshGroups[node.meshGroup];

                for (std::size_t i = 0; i < group.primitives.size(); ++i) {
                    const auto& prim = group.primitives[i];
                    GameObject* target = obj;
                    if (i > 0) {
                        target = scene.createObject("primitive");
                        target->setParent(obj);
                    }
                    auto* mc = target->addComponent<MeshComponent>();
                    mc->setMesh(prim.mesh);
                    mc->setMaterial(prim.material);
                }
            }

            created[nodeIdx] = obj;
            for (int child : node.children)
                self(self, child, obj);
        };

        instance.nodeObjects.reserve(prefab.roots.size());
        for (int r : prefab.roots) {
            spawn(spawn, r, params.parent);
            instance.nodeObjects.push_back(created[r]);
        }
        return instance;
    }
}