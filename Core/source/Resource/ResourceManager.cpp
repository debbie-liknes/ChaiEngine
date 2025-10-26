#include <Resource/ResourceManager.h>

namespace chai
{
	NewAssetManager& NewAssetManager::instance()
	{
		static NewAssetManager manager;
		return manager;
	}

	void NewAssetManager::registerLoader(std::shared_ptr<IAssetLoader> loader)
	{
		m_loaders.push_back(loader);
	}

	std::string NewAssetManager::getExtension(const std::string& file)
	{
		if (size_t dotPosition = file.find_last_of('.'); dotPosition != std::string::npos && dotPosition != 0)
		{
			return file.substr(dotPosition + 1); // Extract extension
		}

		// Return an empty string if no extension is found
		return "";
	}
}