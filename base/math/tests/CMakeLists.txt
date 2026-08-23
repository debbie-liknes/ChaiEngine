project(ChaiMath)

add_executable(${PROJECT_NAME}_tests
        main.cpp
        mat_tests.cpp
        vec_test.cpp
        ops_test.cpp
        quat_tests.cpp
        test_helpers.h
)

target_link_libraries(ChaiMath_tests
        PUBLIC
        gtest
        gmock
        PRIVATE
        ChaiMath
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Base")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)