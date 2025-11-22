#pragma once
#include <Asset/AssetHandle.h>
#include <Asset/AssetManager.h>
#include <Resource/Resource.h>
#include <Resource/ResourcePool.h>

namespace chai
{
    class ResourceManager 
    {
    public:
        static ResourceManager& instance();

		template<typename ResourceType, typename... Args>
        ResourceHandle createFromAsset(AssetHandle assetHandle)
        {
            static_assert(std::is_base_of_v<Resource, ResourceType>,
                "ResourceType must derive from Resource");

			return ResourceHandle(m_resourcePool.add(std::make_unique<ResourceType>(assetHandle)));
		}

        template<typename ResourceType>
        ResourceHandle add(std::unique_ptr<ResourceType> resource)
        {
            static_assert(std::is_base_of_v<Resource, ResourceType>,
                "ResourceType must derive from Resource");
			return ResourceHandle(m_resourcePool.add(std::move(resource)));
        }

		template<typename ResourceType>
        ResourceType* getResource(Handle handle) 
        {
			return dynamic_cast<ResourceType*>(m_resourcePool.get(handle));
		}

    private:
        ResourcePool<Resource> m_resourcePool;
    };
}