# External submodule libraries

add_subdirectory(glm)
add_subdirectory(glfw)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)

set_target_properties(glad PROPERTIES FOLDER "external")
set_target_properties(glm PROPERTIES FOLDER "external")
set_target_properties(glfw PROPERTIES FOLDER "external/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "external/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "external/GLFW3")
set_target_properties(gmock PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gmock_main PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gtest PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gtest_main PROPERTIES FOLDER "external/GoogleTest")