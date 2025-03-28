#pragma once
#include <functional>
#include <string>

namespace chai::kettle
{
    using FactoryFn = std::function<void* ()>;

    class PluginRegistry {
    public:
        static PluginRegistry& Instance();

        // Register a factory under a category and name
        void Register(const std::string& category, const std::string& name, FactoryFn factory);

        // Get a factory by category/name
        FactoryFn Get(const std::string& category, const std::string& name) const;

        // Get all factories of a type (e.g. all physics backends)
        std::vector<std::string> List(const std::string& category) const;

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, FactoryFn>> m_factories;
    };
}