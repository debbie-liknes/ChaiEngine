#include <Components/MeshComponent.h>

namespace chai::cup
{
	MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner)
	{
		materials.push_back(MaterialSystem::createPhong());
	}

	MeshComponent::~MeshComponent()
	{

	}

	void MeshComponent::setMesh(Handle meshAsset)
	{
		meshHandle = meshAsset;
	}
}