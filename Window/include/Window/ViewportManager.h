#pragma once
#include <Window/Viewport.h>
#include <vector>
#include <unordered_map>
#include <Input/EventHandler.h>

namespace chai
{
	class ICamera;
	class ViewportManager : public IEventHandler
	{
	public:
		ViewportManager();
		~ViewportManager() = default;

		// Create a new viewport
		uint64_t createViewport(uint64_t window_id, const ViewportDesc& desc);

		// Get viewport by id
		Viewport* getViewport(uint32_t viewport_id);

		std::vector<Viewport*> getAllViewports() const {
			std::vector<Viewport*> result;
			result.reserve(viewports.size());
			for (const auto& vp : viewports) {
				result.push_back(vp.get());
			}
			return result;
		}

		void assignCameraToViewport(uint32_t viewportId, ICamera* camera) {}

		void handleEvent(const InputEvent& event) override;

		//// Destroy viewport and all its children
		//void destroyViewport(uint32_t viewport_id) {
		//	Viewport* vp = getViewport(viewport_id);
		//	if (!vp) return;

		//	// Recursively destroy children first
		//	auto children_copy = vp->children; // Copy to avoid iterator invalidation
		//	for (uint32_t child_id : children_copy) {
		//		destroyViewport(child_id);
		//	}

		//	// Remove from parent's children list
		//	removeFromParent(viewport_id);

		//	// Find and remove from our containers
		//	size_t index = findViewportIndex(viewport_id);
		//	if (index == SIZE_MAX) return;

		//	// Remove from id_to_index map
		//	id_to_index.erase(viewport_id);

		//	// Update indices for viewports after the removed one
		//	for (size_t i = index + 1; i < viewports.size(); ++i) {
		//		id_to_index[viewports[i]->id] = i - 1;
		//	}

		//	// Remove from vector (swap with last and pop for O(1) removal)
		//	if (index < viewports.size() - 1) {
		//		std::swap(viewports[index], viewports.back());
		//		id_to_index[viewports[index]->id] = index;
		//	}
		//	viewports.pop_back();
		//}

		//// Get all viewports for a specific window
		//std::vector<uint32_t> getViewportsForWindow(uint32_t window_id) const {
		//	std::vector<uint32_t> result;
		//	for (const auto& vp : viewports) {
		//		if (vp->window_id == window_id) {
		//			result.push_back(vp->id);
		//		}
		//	}
		//	return result;
		//}

		//// Set parent-child relationship
		//bool setParentViewport(uint32_t child_id, uint32_t parent_id) {
		//	Viewport* child = getViewport(child_id);
		//	Viewport* parent = getViewport(parent_id);

		//	if (!child) return false;
		//	if (parent_id != 0 && !parent) return false;

		//	// Check for circular dependency
		//	if (parent_id != 0 && wouldCreateCycle(child_id, parent_id)) {
		//		return false;
		//	}

		//	// Remove from old parent
		//	removeFromParent(child_id);

		//	// Set new parent
		//	child->parent_id = parent_id;

		//	// Add to new parent's children
		//	if (parent) {
		//		parent->children.push_back(child_id);
		//	}

		//	return true;
		//}

		//// Get direct children of a viewport
		//std::vector<uint32_t> getChildViewports(uint32_t parent_id) const {
		//	const Viewport* parent = getViewport(parent_id);
		//	return parent ? parent->children : std::vector<uint32_t>();
		//}

		//// Get all viewports in the hierarchy (including self)
		//std::vector<uint32_t> getViewportHierarchy(uint32_t root_id) const {
		//	std::vector<uint32_t> result;
		//	collectHierarchy(root_id, result);
		//	return result;
		//}

		//// Update viewport properties
		//void setViewportBounds(uint32_t viewport_id, float x, float y, float width, float height) {
		//	Viewport* vp = getViewport(viewport_id);
		//	if (vp) {
		//		vp->x = x;
		//		vp->y = y;
		//		vp->width = width;
		//		vp->height = height;
		//	}
		//}

		//// Get total number of viewports
		//size_t getViewportCount() const {
		//	return viewports.size();
		//}

		//// Check if viewport exists
		//bool hasViewport(uint32_t viewport_id) const {
		//	return findViewportIndex(viewport_id) != SIZE_MAX;
		//}

	private:
		//// Helper to check for circular dependencies
		//bool wouldCreateCycle(uint32_t child_id, uint32_t potential_parent_id) const {
		//	uint32_t current = potential_parent_id;
		//	while (current != 0) {
		//		if (current == child_id) return true;
		//		const Viewport* vp = getViewport(current);
		//		current = vp ? vp->parent_id : 0;
		//	}
		//	return false;
		//}

		//// Helper to recursively collect hierarchy
		//void collectHierarchy(uint32_t viewport_id, std::vector<uint32_t>& result) const {
		//	const Viewport* vp = getViewport(viewport_id);
		//	if (!vp) return;

		//	result.push_back(viewport_id);
		//	for (uint32_t child_id : vp->children) {
		//		collectHierarchy(child_id, result);
		//	}
		//}

		std::vector<std::unique_ptr<Viewport>> viewports;
		std::unordered_map<uint32_t, size_t> id_to_index;
		uint32_t next_id = 1;

		// Helper to find viewport index by id
		size_t findViewportIndex(uint32_t viewport_id) const;

		//// Helper to remove child from parent's children list
		//void removeFromParent(uint32_t viewport_id) {
		//	Viewport* vp = getViewport(viewport_id);
		//	if (!vp || vp->parent_id == 0) return;

		//	Viewport* parent = getViewport(vp->parent_id);
		//	if (parent) {
		//		auto& children = parent->children;
		//		children.erase(std::remove(children.begin(), children.end(), viewport_id), children.end());
		//	}
		//}
	};
}