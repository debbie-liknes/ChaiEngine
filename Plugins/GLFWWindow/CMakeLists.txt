# Add source to this project's executable.

set(WindowGLFW_headers
	GLFWBackend.h
	GLFWSystem.h
)

set(WindowGLFW_source
	GLFWBackend.cpp
	GLFWSystem.cpp
)

add_library (GLFWWindowPlugin SHARED 
	${WindowGLFW_headers} 
	${WindowGLFW_source}
)

target_link_libraries(GLFWWindowPlugin PUBLIC WindowModule glfw)

ChaiPluginDefaults(GLFWWindowPlugin "Graphics/Plugins")