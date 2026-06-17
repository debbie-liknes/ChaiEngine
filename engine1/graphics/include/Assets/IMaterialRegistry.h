#pragma once
#include <Assets/MaterialAsset.h>

namespace chai::gfx
{
	class IMaterialRegistry
	{
    public:
        ~IMaterialRegistry() = default;

		virtual Handle<Material> ingest(AssetId, MaterialAsset) = 0;
        virtual Handle<Material> load(AssetId) = 0;
        virtual void release(Handle<Material>) = 0;
        virtual Handle<Material> defaultMaterial() = 0;
	};
}