#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <algorithm>
#include <memory>

#include <Core/Shortcut.h>

namespace chai
{
    /*
    * \description The Action interface is loosely inspired by Qt's QAction
    * for the purpose of providing a decoupled interface for delegating
    * event triggers without tying the mechanism to the UI. Abstracting
    * the mechanism of triggering an action away from the UI also improves
    * the testability of the event, since user interaction is not required
    * to activate it.
    * 
    * \note This may be more closely tied to a signal system in the future.
    */
	class Action
	{
    public:
        Action() = default;
        explicit Action(const std::string& id) : id_(id) {}
        explicit Action(const std::string& id, const std::function<void()>& callback)
            : callback_(callback), id_(id) {}

        /*
        * Triggers the event held by the action.
        */
        void trigger() const;

        const std::vector<std::shared_ptr<Action>>* getChildren() const { return &children_; }
        void setChildren(std::vector<std::shared_ptr<Action>>&& actions) { children_ = actions; }
        void addChild(std::shared_ptr<Action> action) { children_.push_back(action); }
        void attachChild(std::shared_ptr<Action> action)
        {
            if (auto itr = std::find(children_.begin(), children_.end(), action);
                itr == children_.end())
                children_.push_back(action);
        }
        
        void setCallback(const std::function<void()>& callback) { callback_ = callback; }
        std::function<void()> getCallback() const { return callback_; }

        void setShortcut(const Shortcut& shortcut) { shortcut_ = shortcut; }
        Shortcut getShortcut() const { return shortcut_; }

        void setCheckable(bool checkable) { checkable_ = checkable; }
        bool isCheckable() const { return checkable_; }

        /*
         * If action is checkable (has on/off state), toggle will change
         * this state.
         */
        void toggle() { checked_ = !checked_; }
        void setChecked(bool checked) { checked_ = checked; }
        bool isChecked() const { return checked_; }

        /*
        * The "enabled" state of an action defines whether it can currently
        * be triggered by the client.
        */
        void setEnabled(bool enabled) { enabled_ = enabled; }
        bool isEnabled() const { return enabled_; }

        /*
         * If separator is marked true, then this action is considered a
         * menu separator.
         */
        void setSeparator(bool separator) { separator_ = separator; }
        bool isSeparator() const { return separator_; }
        
        /*
        * IDs must be globally unique, as they are managed by the
        * ActionManager for the purpose of providing a global command
        * trigger system.
        */
        void setID(std::string_view id) { id_ = id; }
        std::string getID() const { return id_; }

        void setLabel(std::string_view label) { label_ = label; }
        std::string getLabel() const { return label_; }

    private:
        std::function<void()> callback_ = nullptr;

        std::vector<std::shared_ptr<Action>> children_;

        Shortcut shortcut_;

        bool checkable_ = false;
        bool checked_ = false;
        bool enabled_ = true;
        bool separator_ = false;
        std::string id_;
        std::string label_;

        // TODO: Associate menu item with icon
	};
}
