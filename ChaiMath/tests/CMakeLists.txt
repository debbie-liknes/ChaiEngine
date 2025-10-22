project(ChaiMath)

add_executable(${PROJECT_NAME}_tests
    main.cpp
    mat_tests.cpp
    vec_test.cpp
    ops_test.cpp
    quat_tests.cpp
    test_helpers.h
)

target_link_libraries(${PROJECT_NAME}_tests
    PUBLIC
    gtest
    gmock
    ChaiMath
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Core")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)