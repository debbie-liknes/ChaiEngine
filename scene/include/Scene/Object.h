#pragma once
#include <TypeInfo.h>
#include <TypeRegistry.h>
#include <cstdint>

namespace chai::scene
{
    using ObjectId = uint64_t;

    class Object
    {
    public:
        Object();

        ObjectId id() const;
        virtual TypeInfo* typeInfo() const = 0;

    private:
        ObjectId id_;
    };
} // namespace chai::scene

#define CHAI_OBJECT(Type)                                                                          \
    ::chai::TypeInfo* typeInfo() const override                                                    \
    {                                                                                              \
        return ::chai::TypeRegistry::instance().getType<Type>().get();                             \
    }