# Add source to this project's executable.

set(ChaiGraphics_headers
	include/Coordinate/CoordinateSpace.h
	include/Coordinate/Spherical.h
	include/Coordinate/Euclidean.h
	include/ChaiEngine/IMesh.h
	include/ChaiEngine/IMaterial.h
	include/ChaiEngine/Material.h
	include/ChaiEngine/MaterialSystem.h
	include/ChaiEngine/Light.h
	include/ChaiEngine/Renderer.h
	include/ChaiEngine/PipelineState.h
	include/ChaiEngine/PipelineCache.h
	include/ChaiEngine/ShaderProgram.h
	include/ChaiEngine/ShaderCache.h
	include/ChaiEngine/UniformBuffer.h
	include/ChaiEngine/RenderCommandList.h
	include/ChaiEngine/RenderCommandCollector.h
	include/ChaiEngine/Vertex.h
)

set(ChaiGraphics_source
	source/Coordinate/Euclidean.cpp
	source/Coordinate/Spherical.cpp
	source/ChaiEngine/Material.cpp
	source/ChaiEngine/MaterialSystem.cpp
	source/ChaiEngine/Renderer.cpp
	source/ChaiEngine/RenderCommandCollector.cpp
	source/ChaiEngine/UniformBuffer.cpp
)

add_library (ChaiGraphics SHARED 
	${ChaiGraphics_headers} 
	${ChaiGraphics_source}
)

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core ChaiMath glfw)

source_group("Assets" REGULAR_EXPRESSION ".*/Assets/.*")
source_group("Coordinate" REGULAR_EXPRESSION ".*/Coordinate/.*")

ChaiLibDefaults(ChaiGraphics "Graphics")