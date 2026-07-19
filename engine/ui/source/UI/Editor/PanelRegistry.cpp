#include <UI/Editor/PanelRegistry.h>

namespace chai::ui
{
    void PanelRegistry::registerPanel(PanelDesc desc)
    {
        if (desc.id.empty()) {
            desc.id = desc.displayName;
        }
        panels_[desc.id] = std::move(desc);
    }

    void PanelRegistry::unregisterPanel(const std::string& id)
    {
        panels_.erase(id);
    }

    void PanelRegistry::setPanelVisible(const std::string& id, bool visible)
    {
        if (auto it = panels_.find(id); it != panels_.end())
            it->second.visible = visible;
    }
}