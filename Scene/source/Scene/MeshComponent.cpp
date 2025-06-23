#include <Scene/MeshComponent.h>

namespace chai::cup
{
	MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner)
	{
	}

	MeshComponent::~MeshComponent()
	{

	}

	void MeshComponent::setMesh(std::shared_ptr<brew::MeshAsset> meshAsset) {
		mesh = meshAsset;
	}

	void MeshComponent::render() {
		if (!mesh) return;
		// Render mesh with material
	}
}