# Add source to this project's executable.

set(ChaiGraphics_headers
	include/RenderObjects/RenderObject.h
	include/RenderObjects/Triangle.h
	include/RenderObjects/Cube.h
	include/ChaiEngine/ViewData.h
	include/ChaiEngine/Renderer.h
)

set(ChaiGraphics_source
	source/RenderObjects/RenderObject.cpp
	source/RenderObjects/Cube.cpp
	source/RenderObjects/Triangle.cpp
	source/ChaiEngine/Renderer.cpp
)

#this makes me cry
source_group("Renderables/headers" FILES
	include/RenderObjects/RenderObject.h
	include/RenderObjects/Triangle.h
	include/RenderObjects/Cube.h
)

source_group("Renderables/src" FILES
	source/RenderObjects/RenderObject.cpp
	source/RenderObjects/Cube.cpp
	source/RenderObjects/Triangle.cpp
)
find_package(glm CONFIG REQUIRED)

find_package(glfw3 REQUIRED)
add_library (ChaiGraphics SHARED ${ChaiGraphics_headers} ${ChaiGraphics_source})

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glm::glm glfw ChaiApi Scene)

ChaiLibDefaults(ChaiGraphics "Graphics")