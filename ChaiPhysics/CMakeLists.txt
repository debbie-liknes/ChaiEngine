project(ChaiPhysics)

add_library(${PROJECT_NAME}
    include/ChaiPhysics/ChaiPhysics.h
    source/ChaiPhysics/ChaiPhysics.cpp

    include/ChaiPhysics/Core/dbvh.h
    source/ChaiPhysics/Core/dbvh.cpp

    include/ChaiPhysics/Colliders/BoxCollider.h
    source/ChaiPhysics/Colliders/BoxCollider.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Core
    glm::glm
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Physics")
