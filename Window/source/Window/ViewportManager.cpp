#include <Window/ViewportManager.h>
#include <Input/InputSystem.h>

namespace chai
{
	ViewportManager::ViewportManager()
	{
		InputSystem::instance().subscribe(
			[this](const InputEvent& event) {
				handleEvent(event);
			});
	}

    void ViewportManager::handleEvent(const InputEvent& event)
    {
        if (event.type == InputEventType::FramebufferResize)
        {
            const auto& resizeEv = static_cast<const FrameBufferResize&>(event);
            // If we had multiple viewports inside of a window, this would be where we recalculate
            // For now, get the viewport for the window and update it
            for (auto&& vp : viewports)
            {
                if (vp->getParentWindow() == event.windowId)
                {
                    auto desc = vp->getDesc();
                    vp->setRect(desc.x, desc.y, resizeEv.width, resizeEv.height);
                }
            }
        }
    }

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