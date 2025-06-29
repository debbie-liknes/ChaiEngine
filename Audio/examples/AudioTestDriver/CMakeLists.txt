project(AudioTestDriver)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    AudioEngine
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    AudioEngine
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=$ENV{FMOD_HOME}/api/core/lib/x64;%PATH%")
