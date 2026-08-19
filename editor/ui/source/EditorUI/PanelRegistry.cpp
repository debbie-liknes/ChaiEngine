#include <EditorUI/PanelRegistry.h>

namespace chai::ui
{
    void PanelRegistry::registerPanel(PanelDesc desc)
    {
        if (desc.id.empty()) {
            desc.id = desc.displayName;
        }
        panels_[desc.id] = std::move(desc);
    }

    Panel& PanelRegistry::registerPanel(PanelFactory factory)
    {
        auto panel = factory();
        std::string name = panel->displayName();
        newPanels_.emplace(name, std::move(panel));

        auto* panel = getPanelNew(name);
        if (panel && registedCallback_)
            registedCallback_(*panel);

        return *panel;
    }

    void PanelRegistry::unregisterPanel(const std::string& id)
    {
        panels_.erase(id);
        newPanels_.erase(id);
    }

    void PanelRegistry::setPanelVisible(const std::string& id, bool visible)
    {
        if (auto it = panels_.find(id); it != panels_.end())
            it->second.visible = visible;

        if (auto it = newPanels_.find(id); it != newPanels_.end())
            it->second->visible = visible;
    }

    bool PanelRegistry::isVisible(const std::string& id)
    {
        if (auto it = panels_.find(id); it != panels_.end())
            return it->second.visible;

        if (auto it = newPanels_.find(id); it != newPanels_.end())
            return it->second->visible;

        return false;
    }

    PanelDesc* PanelRegistry::getPanel(const std::string& id)
    {
        if (auto it = panels_.find(id); it != panels_.end())
            return &it->second;
        return nullptr;
    }

    Panel* PanelRegistry::getPanelNew(const std::string& id)
    {
        if (auto it = newPanels_.find(id); it != newPanels_.end())
            return it->second.get();
        return nullptr;
    }

    void PanelRegistry::onPanelRegistered(PanelRegsiteredCallback cb)
    {
        registedCallback_ = cb;
    }
}