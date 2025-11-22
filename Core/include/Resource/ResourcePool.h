#pragma once
#include <Asset/AssetHandle.h>

namespace chai
{
    // ============================================================================
    // RESOURCE MANAGER (Manages GPU objects)
    // ============================================================================
    template<typename T>
    class ResourcePool 
    {
    public:
        // Add resource, get stable handle
        Handle add(std::unique_ptr<T> resource) {
            uint32_t index;
            auto type = std::type_index(typeid(T));

            if (!m_freeList.empty()) {
                // Reuse slot
                index = m_freeList.back();
                m_freeList.pop_back();
                m_resources[index] = std::move(resource);
                m_generations[index]++;  // Increment generation
            }
            else {
                // New slot
                index = static_cast<uint32_t>(m_resources.size());
                m_resources.push_back(std::move(resource));
                m_generations.push_back(0);
            }

            return Handle{ index, m_generations[index], type };
        }

        // Remove resource
        void remove(Handle handle) {
            if (!isValid(handle)) return;

            m_resources[handle.index].reset();  // Delete resource
            m_freeList.push_back(handle.index); // Mark slot as free
            m_generations[handle.index]++;      // Invalidate old handles
        }

        // Get resource (safe!)
        T* get(Handle handle) {
            if (!isValid(handle)) return nullptr;
            return m_resources[handle.index].get();
        }

        const T* get(Handle handle) const {
            if (!isValid(handle)) return nullptr;
            return m_resources[handle.index].get();
        }

        // Check if handle is still valid
        bool isValid(Handle handle) const {
            return handle.index < m_resources.size() &&
                m_generations[handle.index] == handle.generation &&
                m_resources[handle.index] != nullptr;
        }

    private:
        std::vector<std::unique_ptr<T>> m_resources;
        std::vector<uint32_t> m_generations;  // Detect stale handles
        std::vector<uint32_t> m_freeList;     // Reusable slots
    };
}