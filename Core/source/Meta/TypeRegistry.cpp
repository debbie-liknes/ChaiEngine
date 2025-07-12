#include <Meta/TypeRegistry.h>

namespace chai
{
    TypeRegistry& TypeRegistry::instance() {
        static TypeRegistry reg;
        return reg;
    }
}