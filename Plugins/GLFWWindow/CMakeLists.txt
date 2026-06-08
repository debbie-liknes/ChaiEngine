project(GLFWWindowPlugin)

set(${PROJECT_NAME}_headers
        WindowGLFW.h
)

set(${PROJECT_NAME}_source
        WindowGLFW.cpp
	    WindowPlugin.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${GLFWWindowPlugin_headers}
        ${GLFWWindowPlugin_source}
)

target_link_libraries(${PROJECT_NAME} PUBLIC Core PRIVATE glfw)

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")