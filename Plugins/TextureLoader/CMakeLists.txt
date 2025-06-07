# Add source to this project's executable.

set(Loader_headers
	include/Loader/PNGLoader.h
)

set(Loader_source
	source/Loader/PNGLoader.cpp
)

add_library (TextureLoader SHARED 
	${Loader_headers} 
	${Loader_source}
)

target_include_directories(TextureLoader PUBLIC include)
target_link_libraries(TextureLoader PUBLIC Core ChaiGraphics)

ChaiPluginDefaults(TextureLoader "Core/Plugins")