/**
 * @file TypeInfo.h
 */
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <memory>
#include <variant>

namespace chai
{
    /**
     * @brief Holds metadata about a type, including its name, size, methods, and properties.
     */
    class TypeInfo
    {
    public:
        using IconId = const char*;
        IconId icon;
        std::string name;
        std::type_index typeIndex{typeid(void)};
        size_t size = 0;
        std::function<std::shared_ptr<void>()> constructor;

        /**
         * @brief Holds information about a method, including its name, invoker function, parameter
         * types, and return type.
         */
        struct MethodInfo
        {
            std::string name;
            std::function<std::any(void*, const std::vector<std::any> &)> invoker;
            std::vector<std::type_index> paramTypes;
            std::type_index returnType{typeid(void)};
        };

        std::unordered_map<std::string, MethodInfo> methods;

        /**
         * @brief Holds information about a property, including its name, type, getter, and setter
         * functions.
         */
        struct PropertyInfo
        {
            std::string name;
            std::type_index type{typeid(void)};
            std::function<std::any(void*)> getter;
            std::function<void(void*, const std::any &)> setter;
        };

        std::unordered_map<std::string, PropertyInfo> properties;

        using MetaValue = std::variant<bool, int64_t, double, std::string>;
        std::unordered_map<std::string, MetaValue> metaData;

        /**
         * @brief Adds a method to the type information, allowing it to be invoked via reflection.
         */
        template <typename T, typename R, typename... Args>
        void addMethod(const std::string& methodName, R (T::*method)(Args...))
        {
            // Assume all methods are public and non-static
            MethodInfo info;
            info.name = methodName;
            info.returnType = std::type_index(typeid(R));
            info.paramTypes = {std::type_index(typeid(Args))...};

            // Create an invoker that can call the method
            info.invoker = [method](void* obj, const std::vector<std::any>& args) -> std::any
            {
                auto* typedObj = static_cast<T*>(obj);
                if constexpr (sizeof...(Args) == 0)
                {
                    if constexpr (std::is_void_v<R>)
                    {
                        (typedObj->*method)();
                        return std::any{};
                    }
                    else
                    {
                        return std::any{(typedObj->*method)()};
                    }
                }
                else
                {
                    // Handle parameters
                    return invokeWithArgs(typedObj, method, args, std::index_sequence_for < Args...>{});
                }
            };

            methods[methodName] = std::move(info);
        }

        /**
         * @brief Adds a property to the type information, allowing it to be accessed and modified
         * via reflection.
         * @param propName The name of the property.
         * @param member A pointer to the member variable to be added as a property.
         */
        template <typename T, typename PropType>
        void addProperty(const std::string& propName,
                         PropType T::* member)
        {
            PropertyInfo info;
            info.name = propName;
            info.type = std::type_index(typeid(PropType));

            info.getter = [member](void* obj) -> std::any
            {
                T* typedObj = static_cast<T*>(obj);
                return std::any{typedObj->*member};
            };

            info.setter = [member](void* obj, const std::any& value)
            {
                T* typedObj = static_cast<T*>(obj);
                typedObj->*member = std::any_cast<PropType>(value);
            };

            properties[propName] = std::move(info);
        }

        template <typename T, typename R, typename S>
        void
        addProperty(const std::string& propName, R (T::*getter)() const, void (T::*setter)(S))
        {
            using PropType = std::decay_t<R>;
            static_assert(std::is_same_v<PropType, std::decay_t<S>>,
                          "getter and setter must agree on the property type");

            PropertyInfo info;
            info.name = propName;
            info.type = std::type_index(typeid(PropType));

            info.getter = [getter](void* obj) -> std::any {
                return std::any{(static_cast<const T*>(obj)->*getter)()};
            };

            info.setter = [setter](void* obj, const std::any& value) {
                (static_cast<T*>(obj)->*setter)(std::any_cast<PropType>(value));
            };

            auto [it, inserted] = properties.insert_or_assign(propName, std::move(info));
        }

        /**
         * @brief Adds a meta data to the type information
         * via reflection.
         */
        void addMeta(const std::string& key, MetaValue value) { 
            metaData[key] = std::move(value);
        }

    private:

        template <typename T, typename R, typename... Args, size_t... I>
        std::any invokeWithArgs(T* obj, R (T::*method)(Args...),
                                const std::vector<std::any>& args,
                                std::index_sequence<I...>) const
        {
            if constexpr (std::is_void_v<R>)
            {
                (obj->*method)(std::any_cast<Args>(args[I])...);
                return std::any{};
            }
            else
            {
                return std::any{(obj->*method)(std::any_cast<Args>(args[I])...)};
            }
        }
    };
}