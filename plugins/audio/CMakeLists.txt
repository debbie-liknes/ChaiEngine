if (NOT "$ENV{FMOD_HOME}" STREQUAL "")
    find_package(FMOD REQUIRED)
endif ()

if (NOT "${FMOD_LIBRARY}" STREQUAL "")
    project(FmodAudioPlugin)

    add_library(${PROJECT_NAME} SHARED
        FmodAudioEngine.h
        FmodAudioEngine.cpp
        FmodAudioPlugin.cpp
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
        Audio
        Runtime

        PRIVATE
        "${FMOD_LIBRARY}"
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE FMOD_FOUND)

    ChaiPluginDefaults(${PROJECT_NAME} "Plugins")
endif ()
