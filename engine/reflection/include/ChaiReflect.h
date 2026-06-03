/**
 * @file ChaiReflect.h
 * @brief Defines the CHAI_REFLECT macro and related utilities for registering types and their
 * members with the TypeRegistry.
 */
#pragma once
#include <TypeRegistry.h>

/**
 * @brief The CHAI_REFLECT macro defines a struct that registers the specified type with the
 * TypeRegistry and provides a describe function to add properties and methods to the type's
 * TypeInfo.
 */
#define CHAI_REFLECT(Type)                                                                         \
    namespace                                                                                      \
    {                                                                                              \
        struct ChaiReflect_##Type {                                                                \
            using Self = Type;                                                                     \
            static void describe(::chai::TypeInfo& type);                                          \
            ChaiReflect_##Type()                                                                   \
            {                                                                                      \
                auto& reg = ::chai::TypeRegistry::instance();                                      \
                reg.registerType<Type>(#Type);                                                     \
                describe(*reg.getType<Type>());                                                    \
            }                                                                                      \
        };                                                                                         \
        [[maybe_unused]] const ChaiReflect_##Type chaiReflectInstance_##Type{};                    \
    }                                                                                              \
    void ChaiReflect_##Type::describe([[maybe_unused]] ::chai::TypeInfo& type)

#define CHAI_FIELD(member) type.addProperty(#member, &Self::member)
#define CHAI_METHOD(method) type.addMethod(#method, &Self::method)