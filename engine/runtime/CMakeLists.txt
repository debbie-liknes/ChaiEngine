project(Runtime)

set(${PROJECT_NAME}_headers
    include/Runtime/Clock.h
    include/Runtime/Engine.h
    include/Runtime/FileIO.h
    include/Runtime/SystemPaths.h
    include/Plugin/DynamicLibrary.h
    include/Plugin/PluginBase.h
    include/Plugin/PluginContext.h
    include/Plugin/PluginLoader.h
    include/Plugin/PluginMacros.h
    include/Plugin/PluginManager.h
    include/Plugin/ServiceLocator.h
)

set(${PROJECT_NAME}_source
    source/Runtime/Engine.cpp
    source/Runtime/SystemPaths.cpp
    source/Runtime/FileIO.cpp
    source/Plugin/DynamicLibrary.cpp
    source/Plugin/PluginLoader.cpp
    source/Plugin/PluginManager.cpp
    source/Plugin/ServiceLocator.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_BINARY_DIR}/exports/${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    TypeReflection
    Logger
    Asset
    Audio
    Graph
    Graphics
    ImGui
    Scene
    UI
    DiagnosticsUI
)

#TODO: Re-enable when CI is fixed for this
#add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")