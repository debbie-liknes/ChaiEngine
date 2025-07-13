project(AudioEngine)

add_subdirectory(examples)

find_package(FMOD REQUIRED)

add_library(${PROJECT_NAME}
    AudioEngine.h
    AudioEngine.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"

    PRIVATE
    "${FMOD_INCLUDE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    glm::glm

    PRIVATE
    "${FMOD_LIBRARY}"
)

ChaiLibDefaults(${PROJECT_NAME} "Audio")
