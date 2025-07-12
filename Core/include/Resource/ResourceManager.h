#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <Resource/ResourceLoader.h>

namespace chai
{
    class ResourceManager 
    {
    public:
        static ResourceManager& instance();

        void registerLoader(std::shared_ptr<IResourceLoader> loader);

        template <typename T = IResource>
        std::shared_ptr<T> load(const std::string& path) 
        {
			std::string searchPath = RESOURCE_PATH + path;
            // Check cache
			// This implies that every resource is loaded on the main thread
			// TODO: Add support for async loading
			// Most large resource loads (images, models, etc.) should be done on a separate thread
            auto it = m_cache.find(searchPath);
            if (it != m_cache.end()) {
                return std::dynamic_pointer_cast<T>(it->second);
            }

            // Find loader
            auto ext = getExtension(searchPath);
            for (auto& loader : m_loaders) {
                if (loader->canLoad(ext)) {
                    auto resource = loader->load(searchPath);
                    m_cache[searchPath] = resource;
                    return std::dynamic_pointer_cast<T>(resource);
                }
            }

            throw std::runtime_error("No loader found for: " + searchPath);
        }

    private:
        CMap<std::string, std::shared_ptr<IResource>> m_cache;
        std::vector<std::shared_ptr<IResourceLoader>> m_loaders;

        std::string getExtension(const std::string& file);
    };
}