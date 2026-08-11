#pragma once
#include <ChaiReflect.h>

#define CHAI_PLUGIN_ABI_VERSION 1

//Place in a .cpp, not header
#define CHAI_PLUGIN(Type)                                                                          \
    extern "C"                                                                                     \
    {                                                                                              \
        int chaiPluginAbiVersion()                                                                 \
        {                                                                                          \
            return CHAI_PLUGIN_ABI_VERSION;                                                        \
        }                                                                                          \
        ::chai::IPlugin* chaiCreatePlugin()                                                        \
        {                                                                                          \
            return new Type();                                                                     \
        }                                                                                          \
    }
