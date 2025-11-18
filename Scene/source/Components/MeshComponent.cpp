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

	void MeshComponent::setMesh(Handle meshAsset)
	{
		m_meshAsset = meshAsset;

		m_meshResource = Handle();
	}

	Handle MeshComponent::getMeshResource()
	{
		if (!m_meshResource.isValid()) 
		{
			m_meshResource = ResourceManager::instance().createFromAsset<MeshResource>(m_meshAsset);
		}
		return m_meshResource;
	}

	void MeshComponent::setMaterial(Handle materialAsset)
	{
		m_materialInstance = materialAsset;
	}
}