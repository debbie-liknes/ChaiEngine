#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <algorithm>

namespace chai
{
    enum class Modifier : uint8_t
    {
        NONE    = 0,
        CTRL    = 1 << 0,
        SHIFT   = 1 << 1,
        ALT     = 1 << 2,
        SUPER   = 1 << 3 // OS-specific (Win / MacOS key)
    };

    constexpr Modifier operator|(Modifier lhs, Modifier rhs)
    {
        return static_cast<Modifier>(static_cast<std::byte>(lhs) | static_cast<std::byte>(rhs));
    }

    constexpr Modifier operator&(Modifier lhs, Modifier rhs)
    {
        return static_cast<Modifier>(static_cast<std::byte>(lhs) & static_cast<std::byte>(rhs));
    }

    constexpr Modifier operator~(Modifier val)
    {
        return static_cast<Modifier>(~static_cast<std::byte>(val));
    }

    inline Modifier& operator|=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline Modifier& operator&=(Modifier& lhs, Modifier rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline bool hasFlag(Modifier flags, Modifier flag)
    {
        return (flags & flag) == flag;
    }

    struct Shortcut
    {
        Modifier modifier = Modifier::NONE;
        std::optional<char> key;

        std::string modifierStr() const
        {
            using enum Modifier;

            std::string result;

            if (hasFlag(modifier, CTRL))
                result += "Ctrl+";
            if (hasFlag(modifier, SHIFT))
                result += "Shift+";
            if (hasFlag(modifier, ALT))
                result += "Alt+";
            if (hasFlag(modifier, SUPER))
                result += "Super+";

            return result;
        }

        explicit operator std::string() const
        {
            std::string result = modifierStr();
            if (key.has_value()) {
                result += *key;
            }
            return result;
        }

        static std::optional<Shortcut> fromString(std::string_view str)
        {
            Shortcut shortcut;

            // Helper lambda to trim leading/trailing spaces
            auto trim = [](std::string_view s) {
                while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
                    s.remove_prefix(1);
                while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
                    s.remove_suffix(1);
                return s;
            };

            // Split string by '+'
            size_t start = 0;
            size_t end = str.find('+');
            std::vector<std::string_view> tokens;

            while (end != std::string_view::npos) {
                tokens.push_back(trim(str.substr(start, end - start)));
                start = end + 1;
                end = str.find('+', start);
            }
            tokens.push_back(trim(str.substr(start)));

            if (tokens.empty())
                return std::nullopt;

            // Process all tokens except the last one as potential modifiers
            for (size_t i = 0; i < tokens.size(); ++i) {
                auto token = tokens[i];

                // Check if token matches a modifier (case-insensitive conversion)
                std::string tokenLower(token);
                std::transform(tokenLower.begin(),
                               tokenLower.end(),
                               tokenLower.begin(),
                               [](unsigned char c) { return std::tolower(c); });

                if (tokenLower == "ctrl" || tokenLower == "control") {
                    shortcut.modifier |= Modifier::CTRL;
                } else if (tokenLower == "shift") {
                    shortcut.modifier |= Modifier::SHIFT;
                } else if (tokenLower == "alt") {
                    shortcut.modifier |= Modifier::ALT;
                } else if (tokenLower == "super" || tokenLower == "cmd" || tokenLower == "win") {
                    shortcut.modifier |= Modifier::SUPER;
                } else {
                    // If it's not a recognized modifier and it's the last token, treat as key
                    if (i == tokens.size() - 1 && token.length() == 1) {
                        shortcut.key = token[0];
                    } else {
                        // Invalid token or single character key was not at the end
                        return std::nullopt;
                    }
                }
            }

            return shortcut;
        }
    };

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

        Shortcut shortcut_;

        bool checkable_ = false;
        bool checked_ = false;
        bool enabled_ = true;
        bool separator_ = false;
        std::string id_;
        std::string label_;

        // TODO: Associate menu item with icon
        // TODO: Associate menu item with keybinding
	};
}
