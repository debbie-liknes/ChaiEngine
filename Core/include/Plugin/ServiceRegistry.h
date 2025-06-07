#pragma once
#include <Meta/TypeRegistry.h>
#include <memory>
#include <string>
#include <functional>
#include <typeindex>

namespace chai
{
    using FactoryFunc = std::function<std::shared_ptr<void>()>;

    //Register services within the plugin
    class ServiceRegistry 
    {
    public:
        template<typename T>
        void registerService(const std::string& serviceName,
            std::function<std::shared_ptr<T>()> factory,
            const std::string& pluginName = "") {
            auto typeErased = [factory]() -> std::shared_ptr<void> {
                return std::static_pointer_cast<void>(factory());
                };

            services_.insert({ serviceName,  {
                .factory = typeErased,
                .typeIndex = std::type_index(typeid(T)),
                .typeName = typeid(T).name()
            } });

            // Register type if not already registered
            if (!TypeRegistry::instance().getType<T>()) {
                TypeRegistry::instance().registerType<T>(typeid(T).name());
                auto typeInfo = TypeRegistry::instance().getType<T>();
                if (typeInfo && !pluginName.empty()) {
                    typeInfo->pluginName = pluginName;
                }
            }
        }

        template<typename T>
        std::shared_ptr<T> getService(const std::string& serviceName) const {
            auto it = services_.find(serviceName);
            if (it != services_.end() && it->second.typeIndex == std::type_index(typeid(T))) {
                return std::static_pointer_cast<T>(it->second.factory());
            }
            return nullptr;
        }

        // Get service by type name (useful for scripting/reflection)
        std::shared_ptr<void> getServiceByTypeName(const std::string& serviceName,
            const std::string& typeName) const {
            auto it = services_.find(serviceName);
            if (it != services_.end() && it->second.typeName == typeName) {
                return it->second.factory();
            }
            return nullptr;
        }

        std::vector<std::string> getServiceNames() const {
            std::vector<std::string> names;
            for (const auto& [name, _] : services_) {
                names.push_back(name);
            }
            return names;
        }

    private:
        struct ServiceInfo 
        {
            std::function<std::shared_ptr<void>()> factory;
            std::type_index typeIndex;
            std::string typeName;
        };
        std::unordered_map<std::string, ServiceInfo> services_;
    };
}