project(TypeReflection)

add_executable(${PROJECT_NAME}_tests
        main.cpp
        TypeRegistryTests.cpp
)

target_link_libraries(${PROJECT_NAME}_tests
        PUBLIC
        gtest
        gmock
        ${PROJECT_NAME}
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Base")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)