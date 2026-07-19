project(Core)

set(${PROJECT_NAME}_headers
    include/Core/Engine.h
    include/Core/FileIO.h
    include/Core/IInput.h
    include/Core/SystemPaths.h
    include/Core/Updatable.h
    include/Plugin/DynamicLibrary.h
    include/Plugin/PluginBase.h
    include/Plugin/PluginContext.h
    include/Plugin/PluginLoader.h
    include/Plugin/PluginMacros.h
    include/Plugin/ServiceLocator.h
    include/Scene/IScene.h
    include/Scene/ModelRegistry.h
    include/Window/Window.h
    include/Window/WindowTypes.h
)

set(${PROJECT_NAME}_source
    source/Core/Engine.cpp
    source/Core/SystemPaths.cpp
    source/Core/FileIO.cpp
    source/Plugin/DynamicLibrary.cpp
    source/Plugin/PluginLoader.cpp
    source/Plugin/ServiceLocator.cpp
    source/Scene/ModelRegistry.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC TypeReflection Logger Asset Graphics UI)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")