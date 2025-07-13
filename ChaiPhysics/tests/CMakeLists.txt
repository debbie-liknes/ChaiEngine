add_executable(${PROJECT_NAME}_tests
    main.cpp
    dbvh_test.cpp
)

target_link_libraries(${PROJECT_NAME}_tests
    PUBLIC
    gtest
    gmock
    ChaiPhysics
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Physics")
