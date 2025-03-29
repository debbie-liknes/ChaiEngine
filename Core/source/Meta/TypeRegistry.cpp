#include <Meta/TypeRegistry.h>

namespace chai::kettle
{
    TypeRegistry& TypeRegistry::Instance() {
        static TypeRegistry reg;
        return reg;
    }
}