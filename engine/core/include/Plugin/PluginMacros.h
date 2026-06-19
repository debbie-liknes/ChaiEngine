#pragma once
#include <ChaiReflect.h>

#define CHAI_PLUGIN_ABI_VERSION 1

#if defined(_WIN32)
#define CHAI_PLUGIN_API extern "C" __declspec(dllexport)
#else
#endif


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
