project(Graphics)

set(${PROJECT_NAME}_headers
        include/Renderer.h
        include/Vertex.h
        include/MeshAsset.h
        include/FrameRenderData.h
        include/Primitives.h
        include/GraphicsEnums.h
        include/GraphicsMacros.h
        include/CameraData.h
        include/Descriptors.h
)

set(${PROJECT_NAME}_source
        source/Primitives.cpp
)

add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Asset)

#ChaiLibDefaults(${PROJECT_NAME} "Engine")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")