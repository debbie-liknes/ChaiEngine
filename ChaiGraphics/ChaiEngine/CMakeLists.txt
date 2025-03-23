# Add source to this project's executable.

set(ChaiGraphics_headers
	include/ChaiEngine/ViewData.h
	include/ChaiEngine/Renderer.h
	include/ChaiEngine/Shader.h
	include/ChaiEngine/ShaderFactory.h
	include/ChaiEngine/VertexBuffer.h
	include/ChaiEngine/UniformBuffer.h
	include/ChaiEngine/Engine.h
)

set(ChaiGraphics_source
	source/ChaiEngine/Renderer.cpp
	source/ChaiEngine/VertexBuffer.cpp
	source/ChaiEngine/UniformBuffer.cpp
	source/ChaiEngine/Shader.cpp
)

set(ChaiRenderables_headers
	include/RenderObjects/RenderObject.h
	include/RenderObjects/Triangle.h
	include/RenderObjects/Cube.h
)

set(ChaiRenderables_source
	source/RenderObjects/RenderObject.cpp
	source/RenderObjects/Cube.cpp
	source/RenderObjects/Triangle.cpp
)

#IDE friendly groups
source_group("Renderables/headers" FILES ${ChaiRenderables_headers})
source_group("Renderables/src" FILES ${ChaiRenderables_source})

source_group("Graphics/headers" FILES ${ChaiGraphics_headers})
source_group("Graphics/src" FILES ${ChaiGraphics_source})

find_package(glm CONFIG REQUIRED)

find_package(glfw3 REQUIRED)

add_library (ChaiGraphics SHARED 
	${ChaiGraphics_headers} 
	${ChaiRenderables_headers}
	${ChaiGraphics_source}
	${ChaiRenderables_source}
)

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glm::glm glfw Scene)

ChaiLibDefaults(ChaiGraphics "Graphics")