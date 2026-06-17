/**
 * @file IMaterialRegistry.h
 */
#pragma once
#include <Assets/MaterialAsset.h>

namespace chai::gfx
{
    /**
     * @brief Interface to register a Material Asset (CPU data)
     */
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