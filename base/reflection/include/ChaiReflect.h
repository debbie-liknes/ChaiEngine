/**
 * @file ChaiReflect.h
 * @brief Defines the macro and related utilities for registering types and their
 * members with the TypeRegistry
 */
#pragma once
#include <TypeRegistry.h>

template<typename>
struct ChaiReflect;

/**
 * @brief Defines a struct that registers the specified type with the
 * TypeRegistry and provides a describe function to add properties and methods to the
 * TypeInfo.
 */
#define CHAI_REFLECT(Type, TypeName)                                                           \
    template<>                                                                                 \
    struct ChaiReflect<Type> {                                                                 \
        using Self = Type;                                                                     \
        static void describe(::chai::TypeInfo& type);                                          \
        ChaiReflect()                                                                          \
        {                                                                                      \
            auto& reg = ::chai::TypeRegistry::instance();                                      \
            reg.registerType<Type>(TypeName);                                                  \
            describe(*reg.getType<Type>());                                                    \
        }                                                                                      \
        static const ChaiReflect reflect;                                                      \
    };                                                                                         \
    const ChaiReflect<Type> ChaiReflect<Type>::reflect;                                        \
                                                                                               \
    void ChaiReflect<Type>::describe([[maybe_unused]] ::chai::TypeInfo& type)

#define CHAI_FIELD(member) type.addProperty(#member, &Self::member)
#define CHAI_METHOD(method) type.addMethod(#method, &Self::method)
#define CHAI_PROPERTY(label, getter, setter) type.addProperty(label, &Self::getter, &Self::setter)
#define CHAI_META(key, value) type.addMeta(key, value)
#define CHAI_ICON(value) type.icon = value

