#pragma once
#include <Meta/TypeInfo.h>
#include <memory>
#include <typeindex>
#include <Types/CMap.h>

namespace chai
{
    class TypeRegistry
    {
    public:
        static TypeRegistry& instance();

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

            types_[typeName] = typeInfo;
            typesByIndex_[std::type_index(typeid(T))] = typeInfo;
        }

        std::shared_ptr<TypeInfo> getType(const std::string& typeName) const
        {
            auto it = types_.find(typeName);
            return (it != types_.end()) ? it->second : nullptr;
        }

        template <typename T>
        std::shared_ptr<TypeInfo> getType() const
        {
            auto it = typesByIndex_.find(std::type_index(typeid(T)));
            return (it != typesByIndex_.end()) ? it->second : nullptr;
        }

        std::shared_ptr<void> createInstance(const std::string& typeName) const
        {
            auto typeInfo = getType(typeName);
            return typeInfo ? typeInfo->constructor() : nullptr;
        }

    private:
        CMap<std::string, std::shared_ptr<TypeInfo>> types_;
        CMap<std::type_index, std::shared_ptr<TypeInfo>> typesByIndex_;
    };
}