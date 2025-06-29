#pragma once
#include <SceneExport.h>
#include <Scene/ComponentBase.h>
#include <ChaiEngine/MeshAsset.h>

namespace chai::cup
{
	class SCENE_EXPORT RenderableComponent : public Component
	{
	public:
		RenderableComponent(GameObject* owner = nullptr) : Component(owner) {}
		std::shared_ptr<brew::MeshAsset> getMesh() const { return mesh; }

	protected:
		std::shared_ptr<brew::MeshAsset> mesh;
	};
}