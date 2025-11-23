project(AudioEngine)

add_subdirectory(examples)
if (NOT "$ENV{FMOD_HOME}" STREQUAL "")
    find_package(FMOD REQUIRED)
endif ()

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
        ChaiMath

        PRIVATE
        "${FMOD_LIBRARY}"
)

if (NOT "${FMOD_LIBRARY}" STREQUAL "")
    target_compile_definitions(${PROJECT_NAME} PRIVATE FMOD_FOUND)
endif ()

ChaiLibDefaults(${PROJECT_NAME} "Audio")
