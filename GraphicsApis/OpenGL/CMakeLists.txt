
set(OpenGLRenderer_headers
	include/OpenGLRenderer/OpenGLRenderer.h
	include/OpenGLRenderer/OpenGLTexture.h
	include/OpenGLRenderer/OpenGLMesh.h
	include/OpenGLRenderer/OpenGLMaterial.h
	include/OpenGLRenderer/GLShader.h
	include/OpenGLRenderer/GLShaderProgram.h
)

set(OpenGLRenderer_source
	source/OpenGLRenderer/OpenGLRenderer.cpp
	source/OpenGLRenderer/OpenGLTexture.cpp
	source/OpenGLRenderer/OpenGLMesh.cpp
	source/OpenGLRenderer/OpenGLMaterial.cpp
	source/OpenGLRenderer/GLShader.cpp
	source/OpenGLRenderer/GLShaderProgram.cpp
)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/external/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include)

add_library (OpenGLRenderer SHARED ${OpenGLRenderer_headers} ${OpenGLRenderer_source})

target_link_libraries(OpenGLRenderer PUBLIC ChaiGraphics WindowModule PRIVATE glad)
target_include_directories(OpenGLRenderer PUBLIC include)

target_compile_definitions(OpenGLRenderer PRIVATE 
    CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

ChaiPluginDefaults(OpenGLRenderer "Graphics/Platforms")