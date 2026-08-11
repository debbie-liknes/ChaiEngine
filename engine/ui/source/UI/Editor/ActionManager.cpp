#include <UI/Editor/ActionManager.h>

#include <UI/Editor/PanelRegistry.h>

#include <numeric>

namespace chai::ui
{
    // WARNING: Entirely AI generated -- probably should be replaced with something more robust / unit tested.
    // Use with caution...
    size_t levenshteinDistance(std::string_view s1, std::string_view s2)
    {
        const size_t m = s1.size();
        const size_t n = s2.size();
        if (m == 0)
            return n;
        if (n == 0)
            return m;

        std::vector<size_t> dp(n + 1);
        std::iota(dp.begin(), dp.end(), 0);

        for (size_t i = 0; i < m; ++i) {
            size_t prev_diag = dp[0];
            dp[0] = i + 1;
            for (size_t j = 0; j < n; ++j) {
                size_t temp = dp[j + 1];
                if (s1[i] == s2[j]) {
                    dp[j + 1] = prev_diag;
                } else {
                    dp[j + 1] = 1 + std::min({dp[j], dp[j + 1], prev_diag});
                }
                prev_diag = temp;
            }
        }
        return dp[n];
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

    void ActionManager::registerAction(const std::string& id, const std::function<void()>& callback) const
    {
        if (auto action = getAction(id)) {
            action->setCallback(callback);
        }
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
        constexpr size_t maxDistance = 3;

        SearchReturnType results;

        for (const auto& [key, value] : actionDict_) {
            if (levenshteinDistance(input, key) <= maxDistance) {
                results.emplace_back(key, value.get());
            }
        }

        return results;
    }
}
