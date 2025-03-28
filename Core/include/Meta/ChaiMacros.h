#pragma once
#include <Meta/TypeInfo.h>
#include <Meta/TypeRegistry.h>
#include <Meta/MethodInvoker.h>
#include <Plugin/PluginManifest.h>
#include <Plugin/PluginBase.h>

inline std::vector<std::string>& __ChaiRegisteredTypes() {
    static std::vector<std::string> types;
    return types;
}

inline void __ChaiAutoRegisterAllTypes() {
    for (const auto& type : __ChaiRegisteredTypes()) {
        chai::kettle::TypeRegistry::Instance().RegisterType(*chai::kettle::TypeRegistry::Instance().Get(type));
    }
}

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

#define CHAI_PLUGIN(pluginName, versionStr, authorStr, categoryStr, serviceRegFn) \
    class pluginName : public chai::IPluginBase { \
    public: \
        void OnStartup() override { \
            serviceRegFn(); \
            __ChaiAutoRegisterAllTypes(); \
        } \
        const std::string& GetName() const override { \
            return authorStr; \
        } \
        void OnShutdown() override {} \
    }; \
    static chai::PluginManifest manifest = { \
        #pluginName, versionStr, authorStr, categoryStr, __ChaiRegisteredTypes() \
    }; \
    extern "C" __declspec(dllexport) chai::PluginManifest* GetPluginManifest() { \
        return &manifest; \
    } \
    extern "C" __declspec(dllexport) chai::IPluginBase* GetPluginModule() { \
        static pluginName instance; \
        return &instance; \
    }
