#pragma once
#include <functional>
#include <string>
#include <ranges>
#include <UI/Core/Containers/Panel.h>
#include <Containers/Dictionary.h>

namespace chai::ui
{
    using PanelDrawFn = std::function<void()>;
    using PanelFactory = std::function<std::unique_ptr<Panel>()>;
    using PanelRegsiteredCallback = std::function<void(const Panel&)>;

    struct PanelDesc {
        std::string id;
        std::string displayName;
        PanelDrawFn draw;
        bool visible = true;
        // FontWeight titleFont = FontWeight::BoldTitle;
    };

	class PanelRegistry
	{
    public:
        [[deprecated]] void registerPanel(PanelDesc desc);
        Panel& registerPanel(PanelFactory factory);
        void unregisterPanel(const std::string& id);
        void setPanelVisible(const std::string& id, bool visible);
        bool isVisible(const std::string& id);
        [[deprecated]] PanelDesc* getPanel(const std::string& id);
        Panel* getPanelNew(const std::string& id);
        [[deprecated]] auto panels() { return std::views::values(panels_); }
        auto panelsNew() { return std::views::values(newPanels_); }

        //subscribe to this (for the action manager)
        void onPanelRegistered(PanelRegsiteredCallback cb);

    private:
        std::unordered_map<std::string, PanelDesc> panels_;
        Dictionary<std::unique_ptr<Panel>> newPanels_;

        PanelRegsiteredCallback registedCallback_;  //should this be a list? Who else cares?
	};
}