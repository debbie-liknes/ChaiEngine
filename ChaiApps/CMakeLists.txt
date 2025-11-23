project(Chai)

add_executable(${PROJECT_NAME} "Chai.cpp" "Chai.h")

#just use the OpenGL Renderer for now
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiGraphics ChaiPhysics Scene WindowModule)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=$ENV{FMOD_HOME}/api/core/lib/x64;%PATH%")
