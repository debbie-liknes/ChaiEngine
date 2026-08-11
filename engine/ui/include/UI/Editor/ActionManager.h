#pragma once

#include <Action/Action.h>

#include <Containers/Dictionary.h>
#include <memory>
#include <list>

namespace chai::ui
{
    class PanelRegistry;

	class ActionManager
	{
    public:
        explicit ActionManager(PanelRegistry* registry) : panelRegistry_(registry) {}

        [[nodiscard]] std::shared_ptr<Action> getOrCreateAction(const std::string& id);
        [[nodiscard]] std::shared_ptr<Action> getAction(const std::string& id) const;

        void registerAction(const std::string& id, const std::function<void()>& action) const;
        void unregisterAction(const std::string& id) const;

        void registerPanel(const std::string& actionId, const std::string& panelId);
        std::string getPanelId(const std::string& actionId);

		using SearchReturnType = std::vector<std::pair<std::string, Action*>>;
        SearchReturnType fuzzySearch(const std::string_view input) const;
	private:
        PanelRegistry* panelRegistry_ = nullptr;
        Dictionary<std::shared_ptr<Action>> actionDict_;
        Dictionary<std::string> actionId2PanelId_;
	};
}
