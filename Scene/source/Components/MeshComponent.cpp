#include <Components/MeshComponent.h>

namespace chai::cup
{
	MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner)
	{
		materials.push_back(brew::MaterialSystem::createPhong());
	}

	MeshComponent::~MeshComponent()
	{

	}

	void MeshComponent::setMesh(std::shared_ptr<brew::IMesh> meshAsset)
	{
		mesh = meshAsset;
	}
}