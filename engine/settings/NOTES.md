# Engine - Settings Component

- [ ] Define settings format for user styling
- [ ] Define settings format for plugin loading order
- [ ] Set up registry for loading / writing of settings


# Types

Leverages TOML
- Arrays
- Tables
- Inline tables
- Arrays of tables
- Integers & Floats
- Booleans
- Dates & Times, with optional offsets



# Diagrams

```mermaid
---
title: SettingsRegistry
---
classDiagram
    note "settings are stored property-wise"

    class SettingsRegistry {
        + void registerSetting(std::string_view name,)
        + std::optional<std::any> getSetting(std::string_view name)
        + bool hasSetting()

        - std::unordered_map<std::string, std::any> registry_
    }

    PluginContext *-- SettingsRegistry

    class PluginContext {
        + settings : const SettingsRegistry&
    }

```


