#pragma once
#include <Asset/AssetHandle.h>

namespace chai
{
    //class ShaderCache
    //{
    //public:
    //    static ShaderCache& instance();

    //    // Load/compile shader, return handle (deduplicates automatically)
    //    Handle getOrCreate(const std::string& name, const std::vector<std::string>& defines) {
    //        std::string key = name + hashDefines(defines);

    //        auto it = m_cache.find(key);
    //        if (it != m_cache.end()) {
    //            return it->second;  // Already compiled!
    //        }

    //        // Compile new variant
    //        auto shader = compileShader(name, defines);
    //        Handle handle = m_shaderPool.add(std::move(shader));
    //        m_cache[key] = handle;
    //        return handle;
    //    }

    //private:
    //    std::unordered_map<std::string, Handle> m_cache;
    //    ResourcePool<ShaderProgram> m_shaderPool;
    //};
}