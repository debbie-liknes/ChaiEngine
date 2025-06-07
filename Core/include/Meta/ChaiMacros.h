#pragma once
#include <Meta/TypeInfo.h>
#include <Meta/TypeRegistry.h>
#include <Meta/MethodInvoker.h>
#include <Plugin/PluginManifest.h>
#include <Plugin/PluginBase.h>
#include <Plugin/ServiceRegistry.h>
#include <Plugin/PluginRegistry.h>

inline std::vector<std::string>& __ChaiRegisteredTypes() {
    static std::vector<std::string> types;
    return types;
}

//type reflection macros
#define CHAI_SERVICE(ServiceType, ServiceName) \
    services_.registerService<ServiceType>(ServiceName, []() { \
        return std::make_shared<ServiceType>(); \
    }, name_)

#define CHAI_CLASS(type) \
    namespace ChaiInternal_##type { \
        static chai::kettle::TypeInfo BuildTypeInfo() { \
            chai::kettle::TypeInfo info; \
            using propType = type; \
            info.name = #type; \
            info.factory = []() -> void* { return new type(); };

#define CHAI_PROPERTY(prop) \
    info.properties[#prop] = chai::kettle::PropertyInfo{ \
        #prop, \
        [](void* obj) -> std::any { return std::any(static_cast<propType*>(obj)->prop); }, \
        [](void* obj, std::any value) { static_cast<propType*>(obj)->prop = std::any_cast<decltype(propType().prop)>(value); } \
    };

#define CHAI_METHOD(methodPtr) \
    info.methods[#methodPtr] = chai::kettle::MethodInvoker<decltype(&propType::methodPtr)>::Make(#methodPtr, &propType::methodPtr);

#define END_CHAI() \
    __ChaiRegisteredTypes().push_back(info.name); \
    chai::kettle::TypeRegistry::Instance().RegisterType(info); \
    return info; \
    } \
    static struct AutoRegister { \
        AutoRegister() { BuildTypeInfo(); } \
    } _autoRegister; \
    }

#define CHAI_PLUGIN_CLASS(ClassName) \
    class ClassName : public chai::IPlugin { \
    private: \
        chai::ServiceRegistry services_; \
        std::string name_; \
        std::string version_; \
    public: \
        ClassName(const std::string& name, const std::string& version) \
            : name_(name), version_(version) {} \
        const std::string& getName() const override { return name_; } \
        const std::string& getVersion() const override { return version_; } \
        chai::ServiceRegistry& getServices() override { return services_; } \
        void registerServices(); \
        void initialize() override { registerServices(); } \
        void shutdown() override {} \
    }; \
    void ClassName::registerServices()

#define CHAI_REGISTER_PLUGIN(ClassName, PluginName, Version) \
    extern "C" { \
        void RegisterPlugin() { \
            chai::kettle::PluginRegistry::instance().registerPlugin(PluginName, []() { \
                return std::make_unique<ClassName>(PluginName, Version); \
            }); \
        } \
    }
