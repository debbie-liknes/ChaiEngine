project(ImGui)

set(${PROJECT_NAME}_headers
    imgui.h
	imconfig.h
	imgui_internal.h
    backends/imgui_impl_vulkan.h
    backends/imgui_impl_glfw.h
)

set(${PROJECT_NAME}_source
    imgui.cpp
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    backends/imgui_impl_vulkan.cpp
    backends/imgui_impl_glfw.cpp
)

find_package(Vulkan REQUIRED)

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan glfw)

ChaiLibDefaults(${PROJECT_NAME} "external")
