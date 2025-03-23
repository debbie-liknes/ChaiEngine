# Add source to this project's executable.

set(Windowing_headers
	include/Window/GLFWImpl.h
	include/Window/Window.h
	include/Window/Viewport.h
	include/Window/InputState.h
)

set(Windowing_source
	source/Window/GLFWImpl.cpp
	source/Window/Window.cpp
	source/Window/Viewport.cpp
)

find_package(glfw3 REQUIRED)
add_library (WindowModule SHARED ${Windowing_headers} ${Windowing_source})

target_include_directories(WindowModule PUBLIC include)
target_link_libraries(WindowModule PUBLIC Core glfw)

ChaiLibDefaults(WindowModule "Graphics")