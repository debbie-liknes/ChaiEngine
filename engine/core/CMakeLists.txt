project(Core)

set(${PROJECT_NAME}_headers
        include/Core/Engine.h
        include/Core/FileIO.h
        include/Core/SystemPaths.h
        include/Plugin/DynamicLibrary.h
        include/Plugin/PluginBase.h
        include/Plugin/PluginContext.h
        include/Plugin/PluginLoader.h
        include/Plugin/PluginMacros.h
        include/Plugin/ServiceLocator.h
)

set(${PROJECT_NAME}_source
        source/Core/Engine.cpp
        source/Core/SystemPaths.cpp
        source/Core/FileIO.cpp
        source/Plugin/DynamicLibrary.cpp
        source/Plugin/PluginLoader.cpp
        source/Plugin/ServiceLocator.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Core PUBLIC TypeReflection Logger Asset Audio Graphics Scene UI DiagnosticsUI)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")