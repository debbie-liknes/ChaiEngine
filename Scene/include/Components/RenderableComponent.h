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
		Handle getMaterial() const 
		{ 
			//auto found = chai::AssetManager::instance().get<Mesh>(meshHandle, [](const Mesh& m) {
			//	return m.getMaterials();
			//	});

			//return found.value_or({});
			return material;
		}

	protected:
		Handle meshHandle;
		Handle material;
	};
}