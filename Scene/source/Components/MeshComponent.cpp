#include <Components/MeshComponent.h>
#include <Resource/ResourceManager.h>

namespace chai::cup
{
	MeshComponent::MeshComponent(GameObject* owner) : RenderableComponent(owner)
	{
	}

	MeshComponent::~MeshComponent()
	{

	}

	void MeshComponent::setMesh(AssetHandle meshAsset)
	{
		if (!meshAsset.isValid())
		{
			m_meshResource = ResourceManager::instance().createFromAsset<MeshResource>(meshAsset);
		}
	}

	ResourceHandle MeshComponent::getMeshResource()
	{
		return m_meshResource;
	}

	void MeshComponent::setMaterial(ResourceHandle materialInstance)
	{
		m_materialInstance = materialInstance;
	}
}