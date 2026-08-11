#include <UI/Editor/ActionManager.h>

#include <UI/Editor/PanelRegistry.h>

#include <algorithm>
#include <numeric>

namespace chai::ui
{
    // WARNING: Entirely AI generated -- probably should be replaced with something more robust / unit tested.
    // Use with caution... Returns a match score (higher is better). Returns -1 if no
    // match.
    int commandPaletteScore(std::string_view pattern, std::string_view str)
    {
        if (pattern.empty())
            return 0;

        size_t patternIdx = 0;
        int score = 0;
        int consecutiveMatches = 0;
        bool prevWasSeparator = true;

        for (size_t i = 0; i < str.size(); ++i) {
            char strChar = str[i];
            char patternChar = pattern[patternIdx];

            if (std::tolower(strChar) == std::tolower(patternChar)) {
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

    void ActionManager::update(const IInput& input)
    {
        for (auto& [key, value] : actionDict_) {
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

    std::shared_ptr<Action> ActionManager::getAction(const std::string& id) const
    {
        if (auto itr = actionDict_.find(id); itr != actionDict_.end()) {
            return itr->second;
        }
        return nullptr;
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

    void ActionManager::registerPanel(const std::string& actionId, const std::string& panelId)
    {
        registerAction(actionId, [this, panelId]() {
            panelRegistry_->setPanelVisible(panelId, !panelRegistry_->isVisible(panelId));
        });
        actionId2PanelId_.insert_or_assign(actionId, panelId);
    }

    std::string ActionManager::getPanelId(const std::string& actionId)
    {
        if (auto itr = actionId2PanelId_.find(actionId); itr != actionId2PanelId_.end()) {
            return itr->second;
        }
        return std::string();
    }
    
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

        // 1. Filter and score entries
        for (const auto& [key, value] : actionDict_) {
            if (!value->getCallback())
                continue; // Skip all unregistered actions
            int score = commandPaletteScore(input, key);
            if (score > 0 || input.empty()) {
                matches.emplace_back(key, value.get(), score);
            }
        }

        // 2. Sort matches descending by score (highest relevance first)
        std::sort(matches.begin(), matches.end(), [](const ScoredMatch& a, const ScoredMatch& b) {
            if (a.score != b.score) {
                return a.score > b.score; // Higher score first
            }
            return a.key.length() < b.key.length(); // Tie-breaker: shorter string first
        });

        // 3. Convert back to ReturnType (std::vector<std::pair<std::string, Action*>>)
        SearchReturnType results;
        results.reserve(matches.size());
        for (const auto& match : matches) {
            results.emplace_back(match.key, match.value);
        }

        return results;
    }
}
