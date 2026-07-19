#pragma once
#include <UIExport.h>
#include <functional>
#include <string>
#include <ranges>

namespace chai::ui
{
    using PanelDrawFn = std::function<void()>;

    struct PanelDesc {
        std::string id;
        std::string displayName;
        PanelDrawFn draw;
        bool visible = true;
        // FontWeight titleFont = FontWeight::BoldTitle;
    };

	class UI_EXPORT PanelRegistry
	{
    public:
        void registerPanel(PanelDesc desc);
        void unregisterPanel(const std::string& id);
        void setPanelVisible(const std::string& id, bool visible);
        auto panels() { return std::views::values(panels_); }

    private:
        std::unordered_map<std::string, PanelDesc> panels_;
	};
}