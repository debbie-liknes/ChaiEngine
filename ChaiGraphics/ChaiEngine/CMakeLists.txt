# Add source to this project's executable.

set(ChaiGraphics_headers
	include/ChaiEngine/AssetManager.h
	include/ChaiEngine/IMesh.h
	include/ChaiEngine/IMaterial.h
	include/ChaiEngine/Material.h
	include/ChaiEngine/Color.h
	include/ChaiEngine/Light.h
	include/ChaiEngine/MeshAsset.h
	include/ChaiEngine/ViewData.h
	include/ChaiEngine/Renderer.h
	include/ChaiEngine/Shader.h
	include/ChaiEngine/ShaderManager.h
	include/ChaiEngine/VertexBuffer.h
	include/ChaiEngine/UniformBuffer.h
	include/ChaiEngine/RenderCommandList.h
	include/ChaiEngine/RenderCommandCollector.h
	include/ChaiEngine/CoordinateSpace.h
	include/ChaiEngine/Euclidean.h
	include/ChaiEngine/Spherical.h
	include/ChaiEngine/Vertex.h
	include/ChaiEngine/Window.h
)

set(ChaiGraphics_source
	source/ChaiEngine/AssetManager.cpp
	source/ChaiEngine/MeshAsset.cpp
	source/ChaiEngine/Material.cpp
	source/ChaiEngine/Renderer.cpp
	source/ChaiEngine/RenderCommandCollector.cpp
	source/ChaiEngine/VertexBuffer.cpp
	source/ChaiEngine/UniformBuffer.cpp
	source/ChaiEngine/Shader.cpp
	source/ChaiEngine/Euclidean.cpp
	source/ChaiEngine/Spherical.cpp
	source/ChaiEngine/Window.cpp
)

set(ChaiRenderables_headers
	include/Renderables/Renderable.h
	include/Renderables/Triangle.h
	include/Renderables/Cube.h
)

set(ChaiRenderables_source
	source/Renderables/Renderable.cpp
	source/Renderables/Cube.cpp
	source/Renderables/Triangle.cpp
)

#IDE friendly groups
source_group("Renderables/headers" FILES ${ChaiRenderables_headers})
source_group("Renderables/src" FILES ${ChaiRenderables_source})

source_group("Graphics/headers" FILES ${ChaiGraphics_headers})
source_group("Graphics/src" FILES ${ChaiGraphics_source})

add_library (ChaiGraphics SHARED 
	${ChaiGraphics_headers} 
	${ChaiRenderables_headers}
	${ChaiGraphics_source}
	${ChaiRenderables_source}
)

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glm::glm glfw)

ChaiLibDefaults(ChaiGraphics "Graphics")