
set(OpenGLRenderer_headers
        include/OpenGLRenderer/OpenGLRenderer.h
        include/OpenGLRenderer/OpenGLMesh.h
        include/OpenGLRenderer/OpenGLMaterial.h
        include/OpenGLRenderer/OpenGLShader.h
        include/OpenGLRenderer/OpenGLTexture.h
        include/OpenGLRenderer/RenderKey.h
        include/OpenGLRenderer/Uploads.h
        include/OpenGLRenderer/GLHelpers.h
        include/OpenGLRenderer/UniformManager.h
)

set(OpenGLRenderer_source
        source/OpenGLRenderer/OpenGLRenderer.cpp
        source/OpenGLRenderer/OpenGLMesh.cpp
        source/OpenGLRenderer/OpenGLMaterial.cpp
        source/OpenGLRenderer/OpenGLShader.cpp
        source/OpenGLRenderer/Uploads.cpp
        source/OpenGLRenderer/OpenGLTexture.cpp
        include/OpenGLRenderer/GLPipelineState.h
)

add_library(OpenGLRenderer SHARED ${OpenGLRenderer_headers} ${OpenGLRenderer_source})

target_link_libraries(OpenGLRenderer PUBLIC ChaiGraphics WindowModule ChaiMath PRIVATE glad)
if(WIN32)
    target_link_libraries(OpenGLRenderer PRIVATE opengl32 gdi32)
endif()
target_include_directories(OpenGLRenderer PUBLIC include)

target_compile_definitions(OpenGLRenderer PRIVATE
        CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}/\"
)

ChaiPluginDefaults(OpenGLRenderer "Graphics/Platforms")