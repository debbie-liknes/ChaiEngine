#include <Asset/AssetHandle.h>

namespace chai
{
    // In Resource/ResourceFactory.h
    template <typename ResourceT>
    struct ResourceFactory; // primary template, undefined

    //// In Resource/MeshResourceFactory.cpp (or wherever mesh GPU code lives)
    //template <>
    //struct ResourceFactory<MeshResource> {
    //    static std::unique_ptr<MeshResource> build(Handle<MeshAsset> assetHandle)
    //    {
    //        auto* asset = AssetManager::instance().get(assetHandle);
    //        //if (!asset)
    //        //    return nullptr;

    //        //auto resource = std::make_unique<MeshResource>();
    //        //// ... all the vertex layout, interleaving, submesh setup
    //        //// that's currently in MeshComponent ...
    //        //return resource;
    //    }
    //};
}