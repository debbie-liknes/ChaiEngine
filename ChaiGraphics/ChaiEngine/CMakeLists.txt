# Add source to this project's executable.

set(ChaiGraphics_headers
	include/RenderObjects/RenderObject.h
	include/RenderObjects/Triangle.h
	include/RenderObjects/Cube.h
)

set(ChaiGraphics_source
	source/RenderObjects/RenderObject.cpp
	source/RenderObjects/Cube.cpp
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
)

find_package(glfw3 REQUIRED)
add_library (ChaiGraphics SHARED ${ChaiGraphics_headers} ${ChaiGraphics_source})

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glfw ChaiApi)

ChaiLibDefaults(ChaiGraphics "Graphics")