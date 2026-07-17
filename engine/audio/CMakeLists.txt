project(Audio)

set(${PROJECT_NAME}_headers
        include/Audio/IAudioEngine.h
)

add_library(${PROJECT_NAME} INTERFACE
        ${${PROJECT_NAME}_headers}
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} INTERFACE ChaiMath)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")