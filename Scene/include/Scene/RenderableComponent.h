#pragma once
#include <SceneExport.h>
#include <Scene/ComponentBase.h>
#include <ChaiEngine/MeshAsset.h>
#include <ChaiEngine/MaterialAsset.h>
#include <ChaiEngine/Material.h>

namespace chai::cup
{
	class SCENE_EXPORT RenderableComponent : public Component
	{
	public:
		RenderableComponent(GameObject* owner = nullptr) : Component(owner) {}
		std::shared_ptr<brew::IMesh> getMesh() const { return mesh; }
		std::vector<std::shared_ptr<brew::Material>> getMaterials() const { return materials; }

	protected:
		std::shared_ptr<brew::IMesh> mesh;
		std::vector<std::shared_ptr<brew::Material>> materials;
	};
}