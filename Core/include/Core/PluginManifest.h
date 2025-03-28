#pragma once
#include <string>
#include <vector>
#include <functional>

namespace chai
{
    struct PluginManifest {
        std::string name;
        std::string version;
        std::string author;
        std::string category;

        std::vector<std::string> registeredTypes;

        //register callbacks
        std::function<void()> onRegister;

    };
}