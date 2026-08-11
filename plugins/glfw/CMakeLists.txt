project(GLFWWindowPlugin)

set(${PROJECT_NAME}_headers
        WindowGLFW.h
        Input.h
)

set(${PROJECT_NAME}_source
        WindowGLFW.cpp
	    WindowPlugin.cpp
        Input.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${GLFWWindowPlugin_headers}
        ${GLFWWindowPlugin_source}
)

find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC Runtime Window PRIVATE glfw Vulkan::Vulkan)

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")