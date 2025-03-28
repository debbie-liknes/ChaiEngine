#include <Resource/ResourceManager.h>

namespace chai
{
	ResourceManager& ResourceManager::Instance()
	{
		static ResourceManager manager;
		return manager;
	}

	void ResourceManager::RegisterLoader(std::shared_ptr<IResourceLoader> loader)
	{
		m_loaders.push_back(loader);
	}

	std::string ResourceManager::getExtension(std::string& file)
	{
		size_t dotPosition = file.find_last_of('.');

		// If a dot exists and isn't the first character
		if (dotPosition != std::string::npos && dotPosition != 0) {
			return file.substr(dotPosition + 1); // Extract extension
		}

		// Return an empty string if no extension is found
		return "";
	}
}