#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <Resource/ResourceLoader.h>

namespace chai
{
    class ResourceManager {
    public:
        static ResourceManager& Instance();

        void RegisterLoader(std::shared_ptr<IResourceLoader> loader);

        template <typename T = IResource>
        std::shared_ptr<T> Load(const std::string& path) {
            // Check cache
            auto it = m_cache.find(path);
            if (it != m_cache.end()) {
                return std::dynamic_pointer_cast<T>(it->second);
            }

            // Find loader
            auto ext = getExtension(path);
            for (auto& loader : m_loaders) {
                if (loader->CanLoad(ext)) {
                    auto resource = loader->Load(path);
                    m_cache[path] = resource;
                    return std::dynamic_pointer_cast<T>(resource);
                }
            }

            throw std::runtime_error("No loader found for: " + path);
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<IResource>> m_cache;
        std::vector<std::shared_ptr<IResourceLoader>> m_loaders;

        std::string getExtension(std::string& file);
    };
}