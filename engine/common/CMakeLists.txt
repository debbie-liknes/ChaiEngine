project(Core)

set(${PROJECT_NAME}_headers
        include/Plugin/PluginBase.h
        include/Plugin/PluginRegistry.h
        include/Plugin/ServiceLocator.h
        include/Plugin/ServiceRegistry.h
)

set(${PROJECT_NAME}_source
        source/Plugin/PluginRegistry.cpp
        source/Plugin/ServiceLocator.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

#target_link_libraries(${PROJECT_NAME}_source
#        PUBLIC
#        gtest
#        gmock
#        ChaiMath
#)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC TypeReflection)
#target_compile_definitions(Core PUBLIC RESOURCE_PATH=\"${RESOURCE_DIR}\")

ChaiLibDefaults(${PROJECT_NAME} "Engine")