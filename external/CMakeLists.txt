# External submodule libraries

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE "SHARED" CACHE STRING "" FORCE)
add_subdirectory(glfw)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)

set(Python3_FIND_VIRTUALENV ONLY)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SHADERC_ENABLE_SHARED_CRT ON CACHE BOOL "" FORCE)
set(SHADERC_SPIRV_TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/spirv-tools" CACHE PATH "" FORCE)
set(SHADERC_GLSLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glslang" CACHE PATH "" FORCE)
set(SHADERC_SPIRV_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/spirv-headers" CACHE PATH "" FORCE)

set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory(shaderc)

add_subdirectory(imgui)

option(TRACY_ENABLE "" ON)
option(TRACY_ON_DEMAND "" ON)
set(TRACY_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(tracy)

set_target_properties(glad PROPERTIES FOLDER "external")
set_target_properties(glfw PROPERTIES FOLDER "external/GLFW3")
set_target_properties(uninstall PROPERTIES FOLDER "external/GLFW3")
set_target_properties(update_mappings PROPERTIES FOLDER "external/GLFW3")
set_target_properties(gmock PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gmock_main PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gtest PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(gtest_main PROPERTIES FOLDER "external/GoogleTest")
set_target_properties(TracyClient PROPERTIES FOLDER "external/tracy")

function(assign_folder_recursive dir folder_name)
    get_property(targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target ${targets})
        set_target_properties(${target} PROPERTIES FOLDER ${folder_name})
    endforeach()

    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        assign_folder_recursive(${subdir} ${folder_name})
    endforeach()
endfunction()

assign_folder_recursive(${CMAKE_CURRENT_SOURCE_DIR}/shaderc "external/shaderc")
