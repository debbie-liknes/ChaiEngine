# Add source to this project's executable.

set(Windowing_headers
	include/Window/InputState.h
	include/Window/Window.h
	include/Window/WindowManager.h
	include/Window/WindowSystem.h
	include/Window/Viewport.h
	include/Window/ViewportManager.h
)

set(Windowing_source
	source/Window/WindowManager.cpp
	source/Window/WindowSystem.cpp
	source/Window/Window.cpp
	source/Window/Viewport.cpp
	source/Window/ViewportManager.cpp
)

add_library (WindowModule SHARED ${Windowing_headers} ${Windowing_source})

target_include_directories(WindowModule PUBLIC include)
target_link_libraries(WindowModule PUBLIC Core ChaiMath)

ChaiLibDefaults(WindowModule "Window")