#pragma once
#include "CoreExport.h"

#include <Plugin/PluginBase.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <Resource/ResourceHandle.h>

namespace chai
{
    struct PoolEntry 
    {
        std::shared_ptr<IAsset> asset;
        uint64_t generation = 0;

        bool has_value() const {
			return asset != nullptr;
        }
    };

    class AssetPool 
    {
    public:
        AssetPool() = default;

        // Prevent copying
        AssetPool(const AssetPool&) = delete;
        AssetPool& operator=(const AssetPool&) = delete;

        template<class U>
        requires std::derived_from<U, IAsset>
        Handle insert(std::shared_ptr<U> asset) 
        {
            const std::type_index type_id = std::type_index(typeid(*asset));

            if (!free_list_.empty()) 
            {
                size_t index = free_list_.back();
                free_list_.pop_back();

                auto& entry = entries_[index];
                entry.generation++;
                entry.asset = std::move(asset);

                return Handle{ index, entry.generation, type_id };
            }
            else 
            {
                size_t index = entries_.size();
                entries_.push_back(PoolEntry{ std::move(asset), 1 });
                return Handle{ index, 1, type_id };
            }
        }

        template<typename T>
        const T* get(Handle handle) const 
        {
            if (!validate_handle<T>(handle)) return nullptr;

            const auto& entry = entries_[handle.index];
            try 
            {
                auto t = std::dynamic_pointer_cast<T>(entry.asset);
                if (!t)
                {
					std::cerr << "AssetPool::get dynamic_pointer_cast failed for handle index " << handle.index << std::endl;
                    return nullptr;
                }
                return t.get();
                
            }
            catch (...) 
            {
                return nullptr;
            }
        }

    private:
        template<typename T>
        bool validate_handle(Handle handle) const 
        {
            if (handle.index >= entries_.size()) return false;
            if (handle.type != std::type_index(typeid(T))) return false;

            const auto& entry = entries_[handle.index];
            return entry.generation == handle.generation && entry.has_value();
        }

        std::vector<PoolEntry> entries_;
        std::vector<size_t> free_list_;
    };
}