project(EditorUI)

set(${PROJECT_NAME}_headers
    include/EditorUI/SceneHierarchy.h
)

set(${PROJECT_NAME}_source
	source/EditorUI/SceneHierarchy.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE Logger Scene TypeReflection UI)

ChaiLibDefaults(${PROJECT_NAME} "Editor")
