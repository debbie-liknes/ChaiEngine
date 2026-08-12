/**
 * @file TypeRegistry.h
 * @brief Defines the TypeRegistry class which manages the registration and retrieval of type
 * @todo Add some guards to make this thread safe
 * @todo Consider hot reload, especially in the case of things like plugins
 * information
 */
#pragma once
#include <TypeInfo.h>
#include <memory>
#include <typeindex>

namespace chai
{
    class TypeRegistry
    {
    public:
        /**
         * @brief In this case, use a singleton pattern for the TypeRegistry to ensure a single
         * global instance that can be accessed throughout the application.
         * 
         * Be careful not to abuse this pattern
         */
        static TypeRegistry& instance();

        /**
         * @brief Registers a type with the registry, storing its name, type index, size, and a
         * constructor
         */
        template <typename T>
        void registerType(const std::string& typeName)
        {
            auto typeInfo = std::make_shared<TypeInfo>();
            typeInfo->name = typeName;
            typeInfo->typeIndex = std::type_index(typeid(T));
            typeInfo->size = sizeof(T);
            typeInfo->constructor = []() -> std::shared_ptr<void>
            {
                return std::static_pointer_cast<void>(std::make_shared<T>());
            };

            if (auto it = types_.find(typeName); it != types_.end())
                reportCollision(typeName, it->second->typeIndex, std::type_index(typeid(T)));

            types_[typeName] = typeInfo;
            typesByIndex_[std::type_index(typeid(T))] = typeInfo;
        }

        /**
         * @brief Find type info by its name
         */
        std::shared_ptr<TypeInfo> getType(const std::string& typeName) const
        {
            auto it = types_.find(typeName);
            return (it != types_.end()) ? it->second : nullptr;
        }

        /**
         * @brief Find type info by its type index (typeid(T))
         */
        template <typename T>
        std::shared_ptr<TypeInfo> getType() const
        {
            auto it = typesByIndex_.find(std::type_index(typeid(T)));
            return (it != typesByIndex_.end()) ? it->second : nullptr;
        }

        std::shared_ptr<TypeInfo> getType(std::type_index index) const
        {
            auto it = typesByIndex_.find(index);
            return (it != typesByIndex_.end()) ? it->second : nullptr;
        }

        /**
         * @brief Creates an instance of a type by its name using the stored constructor. Returns a
         * shared pointer to the created instance, or nullptr if the type is not found
         */
        std::shared_ptr<void> createInstance(const std::string& typeName) const
        {
            auto typeInfo = getType(typeName);
            return typeInfo ? typeInfo->constructor() : nullptr;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<TypeInfo>> types_;
        std::unordered_map<std::type_index, std::shared_ptr<TypeInfo>> typesByIndex_;

        void reportCollision(const std::string& typeName,
                     std::type_index existing,
                     std::type_index incoming);
    };
}
