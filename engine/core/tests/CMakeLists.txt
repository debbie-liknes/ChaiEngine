project(Core)

add_executable(${PROJECT_NAME}_tests
        main.cpp
        PluginTests.cpp
)

target_link_libraries(Core_tests
        PUBLIC
        gtest
        gmock
        PRIVATE
        Core
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Engine")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)