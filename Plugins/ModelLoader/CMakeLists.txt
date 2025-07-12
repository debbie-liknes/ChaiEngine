# Add source to this project's executable.

set(ModelLoaderSource
	ObjLoader.h
	ObjLoader.cpp
)

add_library (ModelLoader SHARED 
	${ModelLoaderSource} 
)

target_link_libraries(ModelLoader PUBLIC Core ChaiGraphics)

ChaiPluginDefaults(ModelLoader "Core/Plugins")