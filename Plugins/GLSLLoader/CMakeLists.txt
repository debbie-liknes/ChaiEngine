# Add source to this project's executable.

set(Loader_headers
        include/Loader/GLSLLoader.h
)

set(Loader_source
        source/Loader/GLSLLoader.cpp
)

add_library(GLSLLoader SHARED
        ${Loader_headers}
        ${Loader_source}
)

target_include_directories(GLSLLoader PUBLIC include)
target_link_libraries(GLSLLoader PUBLIC Core ChaiGraphics)

ChaiPluginDefaults(GLSLLoader "Core/Plugins")