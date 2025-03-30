
set(OpenGLRenderer_headers
	include/OpenGLRenderer/OpenGLRenderer.h
	include/OpenGLRenderer/OpenGLTexture.h
	include/OpenGLRenderer/GlPipelineState.h
	include/OpenGLRenderer/GLShader.h
	include/OpenGLRenderer/GLShaderProgram.h
	include/OpenGLRenderer/GLViewport.h
)

set(OpenGLRenderer_source
	source/OpenGLRenderer/OpenGLRenderer.cpp
	source/OpenGLRenderer/OpenGLTexture.cpp
	source/OpenGLRenderer/GLShader.cpp
	source/OpenGLRenderer/GLShaderProgram.cpp
	source/OpenGLRenderer/GLViewport.cpp
)

find_package(glad CONFIG REQUIRED)

add_library (OpenGLRenderer SHARED ${OpenGLRenderer_headers} ${OpenGLRenderer_source})

target_link_libraries(OpenGLRenderer PUBLIC ChaiGraphics glad::glad WindowModule)
target_include_directories(OpenGLRenderer PUBLIC include)

target_compile_definitions(OpenGLRenderer PRIVATE 
    CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

ChaiLibDefaults(OpenGLRenderer "Graphics/Platforms")