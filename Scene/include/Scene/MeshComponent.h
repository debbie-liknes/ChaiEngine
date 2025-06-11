#pragma once
#include <SceneExport.h>
#include <Scene/ComponentBase.h>
#include <ChaiEngine/IMesh.h>

namespace chai::cup
{
	class SCENE_EXPORT MeshComponent : public Component
	{
        std::shared_ptr<brew::MeshAsset> mesh;
        //std::shared_ptr<MaterialAsset> material;
        bool castShadows = true;
        bool receiveShadows = true;

    public:
        void setMesh(std::shared_ptr<brew::MeshAsset> meshAsset) { mesh = meshAsset; }
        //void setMaterial(std::shared_ptr<MaterialAsset> mat) { material = mat; }

        void render() {
            if (!mesh) return;
            // Render mesh with material
        }
	};
}