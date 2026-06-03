#include <Plugin/PluginRegistry.h>

template <typename T>
struct PluginRegistrar {
    PluginRegistrar() { PluginRegistry::instance().add(std::make_unique<T>()); }
};

#define CHAI_PLUGIN(Type, Name)                                                                    \
    CHAI_REFLECT(Type)                                                                             \
    {                                                                                              \
        type.tags["plugin"] = Name;                                                                \
    }                                                                                              \
    static inline PluginRegistrar<Type> _chai_plugin_##Type {}