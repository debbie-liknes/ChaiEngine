project(TypeReflection)

set(${PROJECT_NAME}_headers
        include/TypeRegistry.h
)

set(${PROJECT_NAME}_source
        source/TypeRegistry.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

ChaiLibDefaults(${PROJECT_NAME} "Engine")