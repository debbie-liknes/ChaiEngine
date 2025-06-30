# Add source to this project's executable.

set(ChaiScene_headers
	include/Scene/ComponentBase.h
	include/Scene/RenderableComponent.h
	include/Scene/Camera.h
	include/Scene/CameraComponent.h
	include/Scene/CameraController.h
	include/Scene/Controller.h
	include/Scene/ControllerComponent.h
	include/Scene/GameObject.h
	include/Scene/LightComponent.h
	include/Scene/LightSystem.h
	include/Scene/Scene.h
	include/Scene/SceneManager.h
	include/Scene/MeshComponent.h
	include/Scene/TransformComponent.h
)

set(ChaiScene_source
	source/Scene/Camera.cpp
	source/Scene/CameraComponent.cpp
	source/Scene/CameraController.cpp
	source/Scene/ControllerComponent.cpp
	source/Scene/GameObject.cpp
	source/Scene/LightSystem.cpp
	source/Scene/LightComponent.cpp
	source/Scene/MeshComponent.cpp
	source/Scene/TransformComponent.cpp
	source/Scene/Scene.cpp
	source/Scene/SceneManager.cpp
)

add_library (Scene SHARED ${ChaiScene_headers} ${ChaiScene_source})

target_include_directories(Scene PUBLIC include)
target_link_libraries(Scene PUBLIC Core ChaiGraphics)

ChaiLibDefaults(Scene "Scene")