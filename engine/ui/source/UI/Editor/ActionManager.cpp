#include <UI/Editor/ActionManager.h>

#include <UI/Editor/PanelRegistry.h>

#include <filesystem>
#include <algorithm>
#include <numeric>
#include <ranges>

namespace
{
    std::vector<std::string> cumulativeSplit(std::string_view str, char delim)
    {
        std::vector<std::string> result;
        size_t pos = 0;

        // Find each delimiter position and store the prefix
        while ((pos = str.find(delim, pos)) != std::string_view::npos) {
            if (pos > 0) { // Prevents adding an empty prefix if str starts with delim
                result.emplace_back(str.substr(0, pos));
            }
            pos++; // Move past the current delimiter
        }

        // Append the full string as the final prefix
        if (!str.empty()) {
            result.emplace_back(str);
        }

        return result;
    }

    int computeScore(std::string_view pattern, std::string_view str)
    {
        if (pattern.empty())
            return 0;

        size_t patternIdx = 0;
        int score = 0;
        int consecutiveMatches = 0;
        bool prevWasSeparator = true;

        for (auto& strChar : str) {
            if (char patternChar = pattern[patternIdx];
                std::tolower(strChar) == std::tolower(patternChar)) {
                // Base match score
                score += 10;

                // Bonus for consecutive character matches
                score += (consecutiveMatches * 5);
                consecutiveMatches++;

                // Bonus for matching start of word or camelCase
                if (prevWasSeparator || std::isupper(strChar)) {
                    score += 15;
                }

                patternIdx++;
                if (patternIdx == pattern.size()) {
                    // Return total score when full pattern is matched
                    return score;
                }
            } else {
                consecutiveMatches = 0;
            }

            prevWasSeparator = std::ispunct(strChar) || std::isspace(strChar);
        }

        // Pattern was not fully matched as a subsequence
        return -1;
    }
}

namespace chai::ui
{
    ActionManager::ActionManager(const std::filesystem::path& configFile, PanelRegistry* registry)
        : loader_(std::make_unique<ActionConfigLoader>(configFile)), panelRegistry_(registry)
    {
        fromConfig(loader_->getConfig());
    }

    void ActionManager::update(const IInput& input)
    {
        for (const auto& [key, value] : actionDict_) {
            if (value->getShortcut().key.has_value()) {
                // Just opens the command palette until we get further with the shortcut system
                if (input.keyDown(Key::P) && input.keyDown(Key::LeftCtrl) && input.keyDown(Key::LeftShift)) {
                    value->trigger();
                }
            }
        }
    }

    std::shared_ptr<Action> ActionManager::getOrCreateAction(const std::string& id)
    {
        if (auto action = getAction(id)) {
            return action;
        } else {
            // Technically, this whole function could be try_emplace, but I don't want to create a temporary of Action
            // if the key already exists. It would be better if we had something like abseil's flat_hash_map that did a
            // lazy evaluation of the "try" bit, like wrapping it in a lambda.
            return actionDict_.try_emplace(id, std::make_unique<Action>(id)).first->second;
        }
    }

    std::shared_ptr<Action> ActionManager::getOrCreateSeparatorAction()
    {
        const auto action = getOrCreateAction("separator");
        action->setSeparator(true);
        return action;
    }

    std::shared_ptr<Action> ActionManager::getAction(const std::string& id) const
    {
        if (auto itr = actionDict_.find(id); itr != actionDict_.end()) {
            return itr->second;
        }
        return nullptr;
    }

    void ActionManager::generateAncestors(std::shared_ptr<Action> action)
    {
        auto currentChild = action;

        auto prefixes = cumulativeSplit(action->getID(), '.');
        prefixes.pop_back(); // Remove current action

        for (const auto& prefix : prefixes | std::views::reverse) {
            auto parent = getOrCreateAction(prefix);
            parent->attachChild(currentChild);

            currentChild = parent;
        }
    }

    void ActionManager::registerAction(const std::string& id, const std::function<void()>& callback)
    {
        getOrCreateAction(id)->setCallback(callback);
    }

    void ActionManager::unregisterAction(const std::string& id) const
    {
        if (auto action = getAction(id)) {
            action->setCallback(nullptr);
        }
    }

    void ActionManager::registerPanel(const std::string& actionId, const std::string& panelId, bool setActionLabel)
    {
        registerAction(actionId, [this, panelId]() {
            panelRegistry_->setPanelVisible(panelId, !panelRegistry_->isVisible(panelId));
        });

        auto registeredAction = getAction(actionId);

        generateAncestors(registeredAction);

        if (setActionLabel)
            registeredAction->setLabel(panelRegistry_->getPanel(panelId)->displayName);

        actionId2PanelId_.insert_or_assign(actionId, panelId);
    }

    std::string ActionManager::getPanelId(const std::string& actionId)
    {
        if (auto itr = actionId2PanelId_.find(actionId); itr != actionId2PanelId_.end()) {
            return itr->second;
        }
        return std::string();
    }
    
    /*
    * TODO: Write a detailed comment on how this fuzzy search works
    */
    ActionManager::SearchReturnType ActionManager::fuzzySearch(std::string_view input) const
    {
        // Temporary struct to pair matched entries with their score
        struct ScoredMatch {
            std::string key;
            Action* value;
            int score;
        };

        std::vector<ScoredMatch> matches;
        matches.reserve(actionDict_.size());

        // Filter and score entries
        for (const auto& [key, value] : actionDict_) {
            if (!value->getCallback())
                continue; // Skip all unregistered actions
            int score = computeScore(input, key);
            if (score > 0 || input.empty()) {
                matches.emplace_back(key, value.get(), score);
            }
        }

        // Sort matches descending by score (highest relevance first)
        std::sort(matches.begin(), matches.end(), [](const ScoredMatch& a, const ScoredMatch& b) {
            if (a.score != b.score) {
                return a.score > b.score; // Higher score first
            }
            return a.key.length() < b.key.length(); // Tie-breaker: shorter string first
        });

        // Convert back to ReturnType (std::vector<std::pair<std::string, Action*>>)
        SearchReturnType results;
        results.reserve(matches.size());
        for (const auto& match : matches) {
            results.emplace_back(match.key, match.value);
        }

        return results;
    }

    void ActionManager::fromConfig(const ActionConfigData& config)
    {
        auto root = getOrCreateAction("editor");

        for (auto& schema : config.editor) {
            root->addChild(fromSchema(schema));
        }
    }

    std::shared_ptr<Action> ActionManager::fromSchema(const BlueprintItemSchema& schema, const std::string& slug)
    {
        std::shared_ptr<Action> action;
        if (schema.id.has_value()) {
            const auto id = (slug.size() > 0 ? slug + "." : "") + schema.id.value();
            action = getOrCreateAction(id);
            if (schema.label.has_value())
                action->setLabel(schema.label.value());
            if (schema.shortcut.has_value()) {
                if (auto shortcutMaybe = Shortcut::fromString(schema.shortcut.value());
                    shortcutMaybe.has_value()) {
                    action->setShortcut(shortcutMaybe.value());
                }
            }
        } else if (schema.type == "separator") {
            action = getOrCreateSeparatorAction();
        }

        std::vector<std::shared_ptr<Action>> items;
        for (const auto& item : schema.items) {
            items.push_back(fromSchema(item, action->getID()));
        }
        action->setChildren(std::move(items));

        return action;
    }
}
