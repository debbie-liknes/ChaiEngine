project(TypeReflection)

set(${PROJECT_NAME}_headers
        include/TypeRegistry.h
        include/TypeInfo.h
        include/ChaiReflect.h
)

set(${PROJECT_NAME}_source
        source/TypeRegistry.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC Logger)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")