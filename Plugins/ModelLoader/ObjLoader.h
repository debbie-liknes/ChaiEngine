#pragma once
#include <Meta/ChaiMacros.h>
#include <Asset/AssetLoader.h>

namespace chai
{
	class ObjLoader : public IAssetLoader
	{
	public:
		ObjLoader() = default;

		bool canLoad(const std::string& ext) const override;
		std::unique_ptr<IAsset> load(const std::string& path) override;
	};
}


CHAI_PLUGIN_CLASS(ModelLoader) 
{
	CHAI_LOADER(chai::ObjLoader, "obj")
}
CHAI_REGISTER_PLUGIN(ModelLoader, "ModelLoader", "1.0.0")