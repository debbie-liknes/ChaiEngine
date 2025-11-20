#pragma once
#include <ChaiGraphicsExport.h>
#include <ChaiEngine/Material.h>
#include <Resource/ResourceManager.h>
#include <Asset/AssetManager.h>

namespace chai
{
    //class CHAIGRAPHICS_EXPORT IPlatformMaterialManager
    //{
    //public:
    //    virtual ~IPlatformMaterialManager() = default;

    //    // Create platform-specific material from asset
    //    virtual void* createPlatformMaterial(const MaterialAsset* materialAsset) = 0;

    //    // Destroy platform-specific material
    //    virtual void destroyPlatformMaterial(void* platformMaterial) = 0;

    //    // Bind material for rendering
    //    virtual void bindMaterial(void* platformMaterial, const MaterialInstance* instance) = 0;

    //    // Optional: Hot reload support
    //    virtual void reloadPlatformMaterial(void* platformMaterial, const MaterialAsset* materialAsset) = 0;
    //};

   // class CHAIGRAPHICS_EXPORT MaterialSystem
   // {
   // public:
   //     MaterialSystem()
   //     {
   //     }

   //     ~MaterialSystem() 
   //     {}

   //     // Create a material resource from an asset handle
   //     Handle createMaterialResource(Handle assetHandle) 
   //     {
			//return ResourceManager::instance().createFromAsset<MaterialResource>(assetHandle);
   //     }

   //     MaterialResource* getMaterialResource(Handle handle) 
   //     {
			//return ResourceManager::instance().getResource<MaterialResource>(handle);
   //     }

   //     const MaterialResource* getMaterialResource(Handle handle) const 
   //     {
   //         return ResourceManager::instance().getResource<MaterialResource>(handle);
   //     }

   // private:
   // };
}