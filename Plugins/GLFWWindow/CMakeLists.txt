# Add source to this project's executable.

set(WindowGLFW_headers
	GLFWSystem.h
)

set(WindowGLFW_source
	GLFWSystem.cpp
)

add_library (GLFWWindowPlugin SHARED 
	${WindowGLFW_headers} 
	${WindowGLFW_source}
)

target_link_libraries(GLFWWindowPlugin PUBLIC WindowModule glfw)

ChaiPluginDefaults(GLFWWindowPlugin "Window/Plugins")