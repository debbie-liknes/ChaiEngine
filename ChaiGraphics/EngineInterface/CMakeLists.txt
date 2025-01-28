
set(ChaiApi_headers
	include/Engine/Window.h
	include/Engine/Renderer.h
	include/Engine/IRenderObject.h
	include/Engine/IViewer.h
	include/Engine/Viewport.h
	include/Engine/IScene.h
	include/Engine/IPipeline.h
	include/Engine/Common.h
)

set(ChaiApi_source
	source/Engine/Viewport.cpp
	source/Engine/Window.cpp
	source/Engine/Renderer.cpp
)

add_library(ChaiApi SHARED ${ChaiApi_headers} ${ChaiApi_source})

target_include_directories(ChaiApi PUBLIC include)
target_link_libraries(ChaiApi PUBLIC Core)

ChaiLibDefaults(ChaiApi "Graphics")