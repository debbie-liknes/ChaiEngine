project(TypeReflection)

set(${PROJECT_NAME}_headers
        include/ChaiReflect.h
        include/TypeRegistry.h
        include/TypeInfo.h
)

set(${PROJECT_NAME}_source
        source/TypeRegistry.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC Logger)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Base")
