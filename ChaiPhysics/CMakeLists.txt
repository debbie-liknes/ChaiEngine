project(ChaiPhysics)

add_library(${PROJECT_NAME}
    core/dbvh.h
    core/dbvh.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Core
    glm::glm
)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Physics")
