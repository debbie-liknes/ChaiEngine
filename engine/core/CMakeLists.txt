project(Core)

set(${PROJECT_NAME}_headers
    include/Action/Action.h
    include/Action/ActionManager.h
)

set(${PROJECT_NAME}_source
    source/Action/Action.cpp
    source/Action/ActionManager.cpp
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
    Containers
)

ChaiLibDefaults(${PROJECT_NAME} "Engine")