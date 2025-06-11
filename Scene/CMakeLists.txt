# Add source to this project's executable.

set(ChaiScene_headers
	include/Scene/ComponentBase.h
	include/Scene/Camera.h
	include/Scene/GameObject.h
	include/Scene/Light.h
	include/Scene/LogicalViewport.h
	include/Scene/Scene.h
	include/Scene/SceneManager.h
	include/Scene/RenderFrameBuilder.h
	include/Scene/MeshComponent.h
)

set(ChaiScene_source
	source/Scene/Camera.cpp
	source/Scene/Scene.cpp
	source/Scene/SceneManager.cpp
	source/Scene/RenderFrameBuilder.cpp
)

add_library (Scene SHARED ${ChaiScene_headers} ${ChaiScene_source})

target_include_directories(Scene PUBLIC include)
target_link_libraries(Scene PUBLIC Core ChaiGraphics)

ChaiLibDefaults(Scene "Graphics")