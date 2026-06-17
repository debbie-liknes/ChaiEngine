project(Chai)

add_executable(${PROJECT_NAME} "Chai.cpp")

#just use the OpenGL Renderer for now
target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
    Core 
    SpdLogger 
    Graphics
    Scene
    PRIVATE
    $<LINK_LIBRARY:WHOLE_ARCHIVE>)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=$ENV{FMOD_HOME}/api/core/lib/x64;%PATH%")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    CHAI_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets")
