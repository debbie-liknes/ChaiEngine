
set(OpenGLRenderer_headers
        include/OpenGLRenderer/OpenGLRenderer.h
        include/OpenGLRenderer/OpenGLMesh.h
        include/OpenGLRenderer/OpenGLMaterial.h
        include/OpenGLRenderer/OpenGLShader.h
        include/OpenGLRenderer/OpenGLTexture.h
        include/OpenGLRenderer/Uploads.h
        include/OpenGLRenderer/GLHelpers.h
        include/OpenGLRenderer/UniformManager.h
        include/OpenGLRenderer/GBuffer.h
        include/OpenGLRenderer/GBuffPass.h
        include/OpenGLRenderer/GLPipelineState.h
        include/OpenGLRenderer/LightingPass.h
        include/OpenGLRenderer/ForwardPass.h
        include/OpenGLRenderer/RenderPipeline.h
)

set(OpenGLRenderer_source
        source/OpenGLRenderer/OpenGLRenderer.cpp
        source/OpenGLRenderer/OpenGLMesh.cpp
        source/OpenGLRenderer/OpenGLMaterial.cpp
        source/OpenGLRenderer/OpenGLShader.cpp
        source/OpenGLRenderer/Uploads.cpp
        source/OpenGLRenderer/OpenGLTexture.cpp
        source/OpenGLRenderer/RenderPipeline.cpp
        source/OpenGLRenderer/LightingPass.cpp
        source/OpenGLRenderer/GBuffPass.cpp
        source/OpenGLRenderer/ForwardPass.cpp
        include/OpenGLRenderer/SkyboxPass.h
        source/OpenGLRenderer/SkyboxPass.cpp
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