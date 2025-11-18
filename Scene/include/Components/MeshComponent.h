#pragma once
#include <SceneExport.h>
#include <Components/RenderableComponent.h>
#include <ChaiEngine/IMesh.h>
#include <Asset/AssetHandle.h>

namespace chai::cup
{
	class SCENE_EXPORT MeshComponent : public RenderableComponent
	{
    public:
        MeshComponent(GameObject* owner = nullptr);
        ~MeshComponent() override;

        void setMesh(Handle meshAsset);
		void setMaterial(Handle material);
		Handle getMeshResource();

		Handle getMesh() const { return m_meshAsset; }
		Handle getMaterial() const
		{
			//auto found = chai::AssetManager::instance().get<Mesh>(meshHandle, [](const Mesh& m) {
			//	return m.getMaterials();
			//	});

			//return found.value_or({});
			return m_materialInstance;
		}


	private:
		Handle m_meshResource;
		Handle m_meshAsset;
		Handle m_materialInstance;
	
	};
}