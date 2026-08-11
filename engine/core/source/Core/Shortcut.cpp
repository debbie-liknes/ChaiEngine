#include <Core/Shortcut.h>

#include <algorithm>
#include <vector>
#include <string_view>

namespace chai
{
    std::string Shortcut::modifierStr() const
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

    Shortcut::operator std::string() const
    {
        std::string result = modifierStr();
        if (key.has_value()) {
            result += *key;
        }
        return result;
    }

    std::optional<Shortcut> Shortcut::fromString(std::string_view str)
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
}
