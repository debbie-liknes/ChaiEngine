#pragma once
#include <SceneExport.h>
#include <Components/RenderableComponent.h>
#include <ChaiEngine/IMesh.h>

namespace chai::cup
{
	class SCENE_EXPORT MeshComponent : public RenderableComponent
	{
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void setMesh(std::shared_ptr<brew::IMesh> meshAsset);
	};
}