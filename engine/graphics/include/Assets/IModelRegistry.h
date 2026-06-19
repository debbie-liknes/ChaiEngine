/**
 * @file IModelRegistry.h
 */
#pragma once
#include <memory>
#include <Assets/ModelPrefab.h>
#include <filesystem>
#include <AssetCache.h>

namespace chai::gfx
{
    /**
     * @brief Interface for CPU model data bookkeeping
     */
    class IModelRegistry
    {
    public:
        virtual ~IModelRegistry() = default;

        [[nodiscard]] virtual std::shared_ptr<const ModelPrefab>
        load(AssetId id, const std::filesystem::path& path) = 0;

        virtual void release(AssetId id) = 0;
    };
}