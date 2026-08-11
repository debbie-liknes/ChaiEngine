#pragma once

#include <Action/Action.h>
#include <Input/IInput.h>
#include <UI/Editor/ActionConfigLoader.h>

#include <Containers/Dictionary.h>
#include <memory>
#include <list>

namespace chai::ui
{
    class PanelRegistry;

	class ActionManager
	{
    public:
        explicit ActionManager(const std::filesystem::path& configFile, PanelRegistry* registry);

        void update(const IInput& input);

        [[nodiscard]] std::shared_ptr<Action> getOrCreateAction(const std::string& id);
        [[nodiscard]] std::shared_ptr<Action> getOrCreateSeparatorAction();
        [[nodiscard]] std::shared_ptr<Action> getAction(const std::string& id) const;

        void generateAncestors(std::shared_ptr<Action> action);

        void registerAction(const std::string& id, const std::function<void()>& action);
        void unregisterAction(const std::string& id) const;

        void registerPanel(const std::string& actionId,
            const std::string& panelId,
            bool setActionLabel = false);
        std::string getPanelId(const std::string& actionId);

		using SearchReturnType = std::vector<std::pair<std::string, Action*>>;
        SearchReturnType fuzzySearch(const std::string_view input) const;

    private:
        void fromConfig(const ActionConfigData& config);
        [[nodiscard]] std::shared_ptr<Action> fromSchema(const BlueprintItemSchema& schema, const std::string& slug = "");

        std::unique_ptr<ActionConfigLoader> loader_;
        PanelRegistry* panelRegistry_ = nullptr;
        Dictionary<std::shared_ptr<Action>> actionDict_;
        Dictionary<std::string> actionId2PanelId_;
	};
}
