#include <Window/ViewportManager.h>

namespace chai
{
	uint32_t ViewportManager::createViewport(uint64_t window_id, const ViewportDesc& desc) 
	{
		uint64_t id = next_id++;

		auto viewport = std::make_unique<Viewport>(id, desc, window_id);

		size_t index = viewports.size();
		viewports.push_back(std::move(viewport));
		id_to_index[id] = index;

		return id;
	}

	size_t ViewportManager::findViewportIndex(uint32_t viewport_id) const
	{
		auto it = id_to_index.find(viewport_id);
		return (it != id_to_index.end()) ? it->second : SIZE_MAX;
	}

	Viewport* ViewportManager::getViewport(uint32_t viewport_id)
	{
		size_t index = findViewportIndex(viewport_id);
		return (index != SIZE_MAX) ? viewports[index].get() : nullptr;
	}
}