#pragma once
#include <Meta/TypeInfo.h>

namespace chai::kettle
{
    class TypeRegistry {
    public:
        static TypeRegistry& Instance() {
            static TypeRegistry reg;
            return reg;
        }

        void RegisterType(const TypeInfo& info) {
            m_Registry[info.name] = info;
        }

        const TypeInfo* Get(const std::string& name) const {
            auto it = m_Registry.find(name);
            return it != m_Registry.end() ? &it->second : nullptr;
        }

    private:
        std::unordered_map<std::string, TypeInfo> m_Registry;
    };
}
