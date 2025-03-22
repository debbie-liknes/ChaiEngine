# Add source to this project's executable.

set(ChaiScene_headers
	include/Scene/Camera.h
	include/Scene/Scene.h
)

set(ChaiScene_source
	source/Scene/Camera.cpp
	source/Scene/Scene.cpp
)

add_library (Scene SHARED ${ChaiScene_headers} ${ChaiScene_source})

target_include_directories(Scene PUBLIC include)
target_link_libraries(Scene PUBLIC Core ChaiGraphics)

ChaiLibDefaults(Scene "Graphics")