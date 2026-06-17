project(AssetTests)

add_executable(${PROJECT_NAME}_tests
        main.cpp
        MockTexture.h
        TestAssetCache.cpp
)

target_link_libraries(${PROJECT_NAME}_tests
        PUBLIC
        gtest
        gmock
        Asset
)

set_target_properties(${PROJECT_NAME}_tests PROPERTIES FOLDER "Engine")
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}_tests)