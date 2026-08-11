project(AudioTestDriver)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Runtime
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    CHAI_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets")

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=$ENV{FMOD_HOME}/api/core/lib/x64;%PATH%"
    FOLDER "Engine/Audio"
)
    