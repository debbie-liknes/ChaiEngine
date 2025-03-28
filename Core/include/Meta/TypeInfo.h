#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>

namespace chai::kettle
{
    struct PropertyInfo {
        std::string name;
        std::function<std::any(void*)> getter;
        std::function<void(void*, std::any)> setter;
    };

    struct MethodInfo {
        std::string name;
        std::function<std::any(void*, const std::vector<std::any>&)> invoker;
    };

    struct TypeInfo {
        std::string name;
        std::function<void* ()> factory;

        std::unordered_map<std::string, PropertyInfo> properties;
        std::unordered_map<std::string, MethodInfo> methods;
    };

}