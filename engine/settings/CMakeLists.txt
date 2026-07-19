project(Settings)

set(${PROJECT_NAME}_headers
    include/Assets/SettingsAsset.h
    include/Loaders/ISettingsLoader.h
    include/Loaders/TomlSettingsLoader.h
    include/Registry/SettingsRegistry.h
)

set(${PROJECT_NAME}_source
    source/Assets/SettingsAsset.cpp
    source/Loaders/TomlSettingsLoader.cpp
    source/Registry/SettingsRegistry.cpp
)

add_library(${PROJECT_NAME}
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Asset PRIVATE Core Logger)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
