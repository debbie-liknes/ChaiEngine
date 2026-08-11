#include <TypeRegistry.h>
#include <Log.h>

namespace chai
{
    TypeRegistry& TypeRegistry::instance()
    {
        static TypeRegistry reg;
        return reg;
    }

    void TypeRegistry::reportCollision(const std::string& typeName,
                                       std::type_index existing,
                                       std::type_index incoming)
    {
        if (existing == incoming)
        {
            CHAI_LOG_INFO("Re-registering '{}' (same type)", typeName);
        } else {
            CHAI_LOG_INFO("Name collision on '{}': '{}' replaced by '{}'", typeName, existing.name(), incoming.name());
        }
    }
}