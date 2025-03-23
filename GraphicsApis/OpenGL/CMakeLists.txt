
set(OpenGLRenderer_headers
	include/OpenGLRenderer/OpenGLRenderer.h
	include/OpenGLRenderer/GlPipelineState.h
	include/OpenGLRenderer/GLShader.h
)

set(OpenGLRenderer_source
	source/OpenGLRenderer/OpenGLRenderer.cpp
)

find_package(glad CONFIG REQUIRED)

add_library (OpenGLRenderer SHARED ${OpenGLRenderer_headers} ${OpenGLRenderer_source})

target_link_libraries(OpenGLRenderer PUBLIC ChaiGraphics glad::glad WindowModule)
target_include_directories(OpenGLRenderer PUBLIC include)

ChaiLibDefaults(OpenGLRenderer "Graphics/Platforms")