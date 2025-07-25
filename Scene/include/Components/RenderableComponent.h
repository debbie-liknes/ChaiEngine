#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <Assets/MeshAsset.h>
#include <Assets/MaterialAsset.h>
#include <ChaiEngine/Material.h>

namespace chai::cup
{
	class SCENE_EXPORT RenderableComponent : public Component
	{
	public:
		explicit RenderableComponent(GameObject* owner = nullptr) : Component(owner) {}
		std::shared_ptr<brew::IMesh> getMesh() const { return mesh; }
		std::vector<std::shared_ptr<brew::Material>> getMaterials() const { return materials; }

	protected:
		std::shared_ptr<brew::IMesh> mesh;
		std::vector<std::shared_ptr<brew::Material>> materials;
	};
}