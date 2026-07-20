#pragma once
#include <ChaiReflect.h>

#define CHAI_PLUGIN_ABI_VERSION 1

#define CHAI_PLUGIN_API
//#if defined(_WIN32)
//#define CHAI_PLUGIN_API __declspec(dllexport)
//#else
//#define CHAI_PLUGIN_API 
//#endif


//Place in a .cpp, not header
#define CHAI_PLUGIN(Type)                                                                          \
    CHAI_PLUGIN_API int chaiPluginAbiVersion()                                                     \
    {                                                                                              \
        return CHAI_PLUGIN_ABI_VERSION;                                                            \
    }                                                                                              \
    CHAI_PLUGIN_API ::chai::IPlugin* chaiCreatePlugin()                                            \
    {                                                                                              \
        return new Type();                                                                         \
    }
