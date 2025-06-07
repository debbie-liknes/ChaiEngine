#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <memory>

namespace chai
{
    class TypeInfo 
    {
    public:
        std::string name;
        std::type_index typeIndex{ typeid(void) };
        size_t size = 0;
        std::function<std::shared_ptr<void>()> constructor;

        // Method information
        struct MethodInfo {
            std::string name;
            std::function<std::any(void*, const std::vector<std::any>&)> invoker;
            std::vector<std::type_index> paramTypes;
            std::type_index returnType{ typeid(void) };
        };
        std::unordered_map<std::string, MethodInfo> methods;

        // Property information
        struct PropertyInfo {
            std::string name;
            std::type_index type{ typeid(void) };
            std::function<std::any(void*)> getter;
            std::function<void(void*, const std::any&)> setter;
        };
        std::unordered_map<std::string, PropertyInfo> properties;

        // Plugin association
        std::string pluginName;

        template<typename T, typename R, typename... Args>
        void addMethod(const std::string& methodName, R(T::* method)(Args...)) {
            MethodInfo info;
            info.name = methodName;
            info.returnType = std::type_index(typeid(R));
            info.paramTypes = { std::type_index(typeid(Args))... };

            info.invoker = [method](void* obj, const std::vector<std::any>& args) -> std::any {
                T* typedObj = static_cast<T*>(obj);
                if constexpr (sizeof...(Args) == 0) {
                    if constexpr (std::is_void_v<R>) {
                        (typedObj->*method)();
                        return std::any{};
                    }
                    else {
                        return std::any{ (typedObj->*method)() };
                    }
                }
                else {
                    // Handle parameters (simplified for demonstration)
                    return InvokeWithArgs(typedObj, method, args, std::index_sequence_for<Args...>{});
                }
                };

            methods[methodName] = std::move(info);
        }

        template<typename T, typename PropType>
        void addProperty(const std::string& propName,
            PropType T::* member) {
            PropertyInfo info;
            info.name = propName;
            info.type = std::type_index(typeid(PropType));

            info.getter = [member](void* obj) -> std::any {
                T* typedObj = static_cast<T*>(obj);
                return std::any{ typedObj->*member };
                };

            info.setter = [member](void* obj, const std::any& value) {
                T* typedObj = static_cast<T*>(obj);
                typedObj->*member = std::any_cast<PropType>(value);
                };

            properties[propName] = std::move(info);
        }

    private:
        template<typename T, typename R, typename... Args, size_t... I>
        std::any invokeWithArgs(T* obj, R(T::* method)(Args...),
            const std::vector<std::any>& args,
            std::index_sequence<I...>) {
            if constexpr (std::is_void_v<R>) {
                (obj->*method)(std::any_cast<Args>(args[I])...);
                return std::any{};
            }
            else {
                return std::any{ (obj->*method)(std::any_cast<Args>(args[I])...) };
            }
        }
    };
}