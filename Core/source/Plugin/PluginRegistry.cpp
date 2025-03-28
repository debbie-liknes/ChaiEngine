#include <Plugin/PluginRegistry.h>
#include <stdexcept>

namespace chai::kettle
{
    PluginRegistry& PluginRegistry::Instance() 
    {
        static PluginRegistry reg;
        return reg;
    }

    // Register a factory under a category and name
    void PluginRegistry::Register(const std::string& category, const std::string& name, FactoryFn factory)
    {
        m_factories[category][name] = factory;
    }

    // Get a factory by category/name
    FactoryFn PluginRegistry::Get(const std::string& category, const std::string& name) const
    {
        auto catIt = m_factories.find(category);
        if (catIt == m_factories.end()) {
            throw std::runtime_error("PluginRegistry: Unknown category: " + category);
        }

        const auto& namedMap = catIt->second;
        auto it = namedMap.find(name);
        if (it == namedMap.end()) {
            throw std::runtime_error("PluginRegistry: Unknown plugin [" + name + "] in category [" + category + "]");
        }

        return it->second;
    }

    // Get all factories of a type
    std::vector<std::string> PluginRegistry::List(const std::string& category) const
    {
        std::vector<std::string> names;

        auto it = m_factories.find(category);
        if (it != m_factories.end()) {
            for (const auto& [name, _] : it->second) {
                names.push_back(name);
            }
        }

        return names;
    }
}