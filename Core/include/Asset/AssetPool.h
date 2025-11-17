#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <Asset/AssetHandle.h>
#include <Asset/AssetLoader.h>

namespace chai
{
   // struct PoolEntry 
   // {
   //     std::unique_ptr<IAsset> asset;
   //     uint32_t generation = 0;

   //     bool has_value() const {
			//return asset != nullptr;
   //     }
   // };

   // class AssetPool 
   // {
   // public:
   //     AssetPool() = default;

   //     // Prevent copying
   //     AssetPool(const AssetPool&) = delete;
   //     AssetPool& operator=(const AssetPool&) = delete;

   //     template<class U>
   //     requires std::derived_from<U, IAsset>
   //     Handle insert(std::unique_ptr<U> asset) 
   //     {
   //         //const std::type_index type_id = std::type_index(typeid(*asset));

   //         if (!free_list_.empty()) 
   //         {
   //             size_t index = free_list_.back();
   //             free_list_.pop_back();

   //             auto& entry = entries_[index];
   //             entry.generation++;
   //             entry.asset = std::move(asset);

   //             return Handle{ index, entry.generation };
   //         }
   //         else 
   //         {
   //             size_t index = entries_.size();
   //             entries_.push_back(PoolEntry( std::move(asset), 1 ));
   //             return Handle{ index, 1 };
   //         }
   //     }

   //     template<typename T>
   //     const T* get(Handle handle) const 
   //     {
   //         if (!validate_handle<T>(handle)) return nullptr;

   //         const auto& entry = entries_[handle.index];
   //         if (!entry.asset) return nullptr;

   //         return dynamic_cast<const T*>(entry.asset.get());
   //     }

   // private:
   //     template<typename T>
   //     bool validate_handle(Handle handle) const 
   //     {
   //         if (handle.index >= entries_.size()) return false;
   //         //if (handle.type != std::type_index(typeid(T))) return false;

   //         const auto& entry = entries_[handle.index];
   //         return entry.generation == handle.generation && entry.has_value();
   //     }

   //     std::vector<PoolEntry> entries_;
   //     std::vector<uint32_t> free_list_;
   // };
}