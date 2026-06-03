#include <TypeRegistry.h>
#include <iostream>

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
            std::cerr << "[TypeRegistry] note: re-registering '" << typeName << "' (same type)\n";
        else
            std::cerr << "[TypeRegistry] WARNING: name collision on '" << typeName
                      << "': " << existing.name() << " replaced by " << incoming.name() << "\n";
    }
}