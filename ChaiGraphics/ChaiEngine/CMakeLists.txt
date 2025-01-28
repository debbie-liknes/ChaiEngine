# Add source to this project's executable.

set(ChaiGraphics_headers
	include/ChaiEngine/ChaiWindow.h
	include/ChaiEngine/Viewer.h
	include/RenderObjects/RenderObject.h
	include/RenderObjects/Triangle.h
)

set(ChaiGraphics_source
	source/ChaiEngine/ChaiWindow.cpp
	source/ChaiEngine/Viewer.cpp
	source/RenderObjects/RenderObject.cpp
	source/RenderObjects/Triangle.cpp
)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
add_library (ChaiGraphics SHARED ${ChaiGraphics_headers} ${ChaiGraphics_source})

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glfw ChaiApi)

ChaiLibDefaults(ChaiGraphics "Graphics")