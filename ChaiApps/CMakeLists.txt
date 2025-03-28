project(Chai)

add_executable (${PROJECT_NAME} "Chai.cpp" "Chai.h")

#just use the OpenGL Renderer for now
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiGraphics Scene WindowModule)
