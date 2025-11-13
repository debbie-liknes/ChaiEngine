#pragma once

namespace chai
{
	class ResourceManager
	{
	public:
		static ResourceManager& instance();

		//void addResource(const std::string& name, std::shared_ptr<IAsset> resource)
		//{
		//	resources_[name] = resource;
		//}

	private:
		ResourceManager() = default;
		~ResourceManager() = default;
		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		//std::unordered_map<std::string, std::shared_ptr<IAsset>> resources_;
	};
}