#include <Asset/AssetManager.h>

namespace chai
{
    AssetManager& AssetManager::instance()
    {
        static AssetManager manager;
        return manager;
    }

    AssetManager::~AssetManager()
    {
        m_loaders.clear();
    }


    void AssetManager::registerLoader(std::shared_ptr<IAssetLoader> loader)
    {
        m_loaders.push_back(loader);
    }

    std::string AssetManager::getExtension(const std::string& file)
    {
        if (const size_t dotPosition = file.find_last_of('.'); dotPosition != std::string::npos && dotPosition != 0)
        {
            return file.substr(dotPosition + 1); // Extract extension
        }

        // Return an empty string if no extension is found
        return "";
    }
}