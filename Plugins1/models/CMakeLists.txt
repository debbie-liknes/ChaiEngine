# Add source to this project's executable.

set(ModelLoaderSource
        ObjLoader.h
        ObjLoader.cpp
        GLTFLoader.h
        GLTFLoader.cpp
        ModelLoaderPlugin.cpp
)

add_library(ModelLoader SHARED
        ${ModelLoaderSource}
)

target_link_libraries(ModelLoader PUBLIC Core ChaiMath Graphics)

ChaiPluginDefaults(ModelLoader "Plugins")