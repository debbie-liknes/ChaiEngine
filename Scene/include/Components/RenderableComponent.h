#pragma once
#include <SceneExport.h>
#include <Components/ComponentBase.h>
#include <ChaiEngine/Material.h>
#include <ChaiEngine/IMesh.h>

namespace chai::cup
{
	class SCENE_EXPORT RenderableComponent : public Component
	{
	public:
		explicit RenderableComponent(GameObject* owner = nullptr) : Component(owner) {}
		Handle getMesh() const { return meshHandle; }
		std::vector<std::shared_ptr<Material>> getMaterials() const { return materials; }

	protected:
		Handle meshHandle;
		std::vector<std::shared_ptr<Material>> materials;
	};
}