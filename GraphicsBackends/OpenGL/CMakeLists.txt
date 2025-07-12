
set(OpenGLRenderer_headers
	include/OpenGLRenderer/OpenGLRenderer.h
	include/OpenGLRenderer/OpenGLMesh.h
	include/OpenGLRenderer/OpenGLMaterial.h
)

set(OpenGLRenderer_source
	source/OpenGLRenderer/OpenGLRenderer.cpp
	source/OpenGLRenderer/OpenGLMesh.cpp
	source/OpenGLRenderer/OpenGLMaterial.cpp
)

add_library (OpenGLRenderer SHARED ${OpenGLRenderer_headers} ${OpenGLRenderer_source})

target_link_libraries(OpenGLRenderer PUBLIC ChaiGraphics WindowModule ChaiMath PRIVATE glad)
target_include_directories(OpenGLRenderer PUBLIC include)

target_compile_definitions(OpenGLRenderer PRIVATE 
    CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}/\"
)

ChaiPluginDefaults(OpenGLRenderer "Graphics/Platforms")