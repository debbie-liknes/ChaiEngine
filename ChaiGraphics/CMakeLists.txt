# Add source to this project's executable.

set(ChaiGraphics_headers
	include/ChaiGraphics/ChaiWindow.h
	include/ChaiGraphics/Common.h
	include/ChaiGraphics/IRenderer.h
	include/ChaiGraphics/Window.h
	include/ChaiGraphics/Viewport.h
	include/ChaiGraphics/Viewer.h
)

set(ChaiGraphics_source
	source/ChaiGraphics/ChaiWindow.cpp
	source/ChaiGraphics/IRenderer.cpp
	source/ChaiGraphics/Window.cpp
	source/ChaiGraphics/Viewer.cpp
	source/ChaiGraphics/Viewport.cpp
)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
add_library (ChaiGraphics SHARED ${ChaiGraphics_headers} ${ChaiGraphics_source})

target_include_directories(ChaiGraphics PUBLIC include)
target_link_libraries(ChaiGraphics PUBLIC Core glfw)

ChaiLibDefaults(ChaiGraphics "Graphics")