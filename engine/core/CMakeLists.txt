project(Core)

set(${PROJECT_NAME}_headers
        include/Plugin/PluginBase.h
        include/Plugin/PluginContext.h
        include/Plugin/PluginMacros.h
        include/Plugin/PluginRegistry.h
        include/Plugin/ServiceLocator.h
        include/Engine.h
)

set(${PROJECT_NAME}_source
        source/Plugin/PluginRegistry.cpp
        source/Plugin/ServiceLocator.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC TypeReflection)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")