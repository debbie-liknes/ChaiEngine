project(Core)

set(${PROJECT_NAME}_headers
    include/Core/Action.h
    include/Core/Shortcut.h
)

set(${PROJECT_NAME}_source
    source/Core/Action.cpp
    source/Core/Shortcut.cpp
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
)

ChaiLibDefaults(${PROJECT_NAME} "Engine")