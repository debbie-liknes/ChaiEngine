project(Core)

set(${PROJECT_NAME}_headers
        include/Plugin/DynamicLibrary.h
        include/Plugin/PluginBase.h
        include/Plugin/PluginContext.h
        include/Plugin/PluginLoader.h
        include/Plugin/PluginMacros.h
        include/Plugin/ServiceLocator.h
	    include/Window/Window.h
	    include/Window/WindowTypes.h
        include/Engine.h
        include/SystemPaths.h
        include/Updatable.h
)

set(${PROJECT_NAME}_source
        source/Plugin/DynamicLibrary.cpp
        source/Plugin/PluginLoader.cpp
        source/Plugin/ServiceLocator.cpp
        source/Engine.cpp
        source/SystemPaths.cpp
)

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC TypeReflection Logger)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")