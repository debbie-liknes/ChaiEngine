project(Graph)

set(${PROJECT_NAME}_headers
    include/Graph/Algorithms.h
)

add_library(${PROJECT_NAME} INTERFACE
    ${${PROJECT_NAME}_headers}
)

target_include_directories(${PROJECT_NAME} INTERFACE include)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Base")
