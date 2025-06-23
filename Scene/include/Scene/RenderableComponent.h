#pragma once
#include <SceneExport.h>
#include <Scene/ComponentBase.h>
#include <ChaiEngine/IMesh.h>
#include <ChaiEngine/IMaterial.h>

namespace chai::cup
{
	class SCENE_EXPORT RenderableComponent : public Component
	{
	public:
		RenderableComponent(GameObject* owner = nullptr) : Component(owner) {}
		std::shared_ptr<brew::MeshAsset> getMesh() const { return mesh; }
		std::shared_ptr<brew::IMaterial> getMaterial() const { return material; }

	protected:
		std::shared_ptr<brew::MeshAsset> mesh;
		std::shared_ptr<brew::IMaterial> material;
	};
}