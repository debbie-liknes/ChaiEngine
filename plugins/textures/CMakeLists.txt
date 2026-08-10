# Add source to this project's executable.

set(Loader_headers
        include/PNGLoader.h
)

set(Loader_source
        source/PNGLoader.cpp
        source/TextureLoaderPlugin.cpp
)

add_library(TextureLoader SHARED
        ${Loader_headers}
        ${Loader_source}
)

target_include_directories(TextureLoader PUBLIC include)
target_link_libraries(TextureLoader PUBLIC Graphics Runtime)

ChaiPluginDefaults(TextureLoader "Plugins")