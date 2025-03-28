#pragma once
#include <Meta/TypeInfo.h>

namespace chai::kettle
{
    // General fallback: unsupported signature
    template <typename>
    struct MethodInvoker;

    // Specialization for 0 args
    template <typename Class>
    struct MethodInvoker<void (Class::*)()> {
        static MethodInfo Make(const std::string& name, void (Class::* method)()) {
            return {
                name,
                [method](void* obj, const std::vector<std::any>&) -> std::any {
                    (static_cast<Class*>(obj)->*method)();
                    return {};
                }
            };
        }
    };

    // Specialization for 1 arg
    template <typename Class, typename A0>
    struct MethodInvoker<void (Class::*)(A0)> {
        static MethodInfo Make(const std::string& name, void (Class::* method)(A0)) {
            return {
                name,
                [method](void* obj, const std::vector<std::any>& args) -> std::any {
                    (static_cast<Class*>(obj)->*method)(std::any_cast<A0>(args[0]));
                    return {};
                }
            };
        }
    };
}