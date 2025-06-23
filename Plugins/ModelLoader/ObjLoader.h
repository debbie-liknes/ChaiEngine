#pragma once
#include <ModelLoaderExport.h>
#include <Meta/ChaiMacros.h>
#include <Resource/ResourceLoader.h>

namespace chai
{
	class ObjLoader : public IResourceLoader {
	public:
		ObjLoader();

		bool canLoad(const std::string& ext) const override;
		std::shared_ptr<IResource> load(const std::string& path) override;
	};
}


CHAI_PLUGIN_CLASS(ModelLoader) {
	CHAI_LOADER(chai::ObjLoader, "obj");
}
CHAI_REGISTER_PLUGIN(ModelLoader, "ModelLoader", "1.0.0")