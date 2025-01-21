project(Chai)

add_executable (${PROJECT_NAME} "Chai.cpp" "Chai.h")


target_link_libraries(${PROJECT_NAME} ChaiGraphics)
