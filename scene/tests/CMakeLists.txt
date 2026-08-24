project(Scene)

add_executable(${PROJECT_NAME}_tests
        main.cpp
        ObjectTableTests.cpp
)

target_link_libraries(${PROJECT_NAME}_tests
        PUBLIC
        gtest
        gmock
        PRIVATE
        Scene
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Scene")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)