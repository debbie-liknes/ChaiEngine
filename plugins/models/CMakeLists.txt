# Add source to this project's executable.

set(ModelLoaderSource
        GLTFLoader.h
        GLTFLoader.cpp
        ModelLoaderPlugin.cpp
)

add_library(ModelLoader SHARED
        ${ModelLoaderSource}
)

target_link_libraries(ModelLoader PUBLIC Runtime ChaiMath Graphics)

ChaiPluginDefaults(ModelLoader "Plugins")