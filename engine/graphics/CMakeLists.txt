project(Graphics)

set(${PROJECT_NAME}_headers
        include/Assets/DefaultTextures.h
        include/Assets/IMeshRegistry.h
        include/Assets/ITextureRegistry.h
        include/Assets/MeshAsset.h
        include/Assets/PrimitiveMeshes.h
        include/Assets/TextureAsset.h
        include/Assets/TextureFormat.h
        include/Common/GraphicsEnums.h
        include/Common/GraphicsMacros.h
        include/Geometry/Vertex.h
        include/Loaders/ITextureLoader.h
        include/Rendering/CameraData.h
        include/Rendering/FrameRenderData.h
        include/Rendering/IRenderer.h
        include/Rendering/LightData.h
)

set(${PROJECT_NAME}_source
        source/PrimitiveMeshes.cpp
        source/DefaultTextures.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Asset)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
