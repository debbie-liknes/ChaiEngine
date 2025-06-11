#pragma once
#include <Meta/TypeInfo.h>
#include <Meta/TypeRegistry.h>
#include <Meta/MethodInvoker.h>
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

#define CHAI_SERVICE_AS(InterfaceType, ConcreteType, ServiceName) \
    services_.registerServiceAs<InterfaceType, ConcreteType>(ServiceName, []() { \
        return std::make_shared<ConcreteType>(); \
    })

#define CHAI_CLASS(ClassName) \
    namespace { \
        struct ClassName##_TypeRegistration { \
            ClassName##_TypeRegistration() { \
                auto& typeReg = chai::TypeRegistry::instance(); \
                typeReg.registerType<ClassName>(#ClassName); \
                auto typeInfo = typeReg.getType<ClassName>(); \
                registerTypeInfo_##ClassName(typeInfo); \
            } \
        }; \
        static ClassName##_TypeRegistration ClassName##_reg; \
    } \
    void registerTypeInfo_##ClassName(std::shared_ptr<chai::TypeInfo> typeInfo)

#define CHAI_IMPLEMENTS(ClassName, InterfaceName) \
    typeInfo->addInterface<InterfaceName>()

#define CHAI_ATTRIBUTE(Key, Value) \
    typeInfo->setAttribute(Key, Value)

#define CHAI_METHOD(ClassName, MethodName) \
    typeInfo->addMethod(#MethodName, &ClassName::MethodName)

#define CHAI_PROPERTY(ClassName, PropName) \
    typeInfo->addProperty(#PropName, &ClassName::PropName)

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
