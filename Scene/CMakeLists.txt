# Add source to this project's executable.

set(ChaiScene_headers
	include/Components/CameraComponent.h
	include/Components/ComponentBase.h
	include/Components/ControllerComponent.h
	include/Components/LightComponent.h
	include/Components/MeshComponent.h
	include/Components/RenderableComponent.h
	include/Components/TransformComponent.h
	include/Controllers/CameraController.h
	include/Controllers/Controller.h
	include/Scene/Camera.h
	include/Scene/GameObject.h
	include/Scene/Scene.h
	include/Scene/SceneManager.h
)

set(ChaiScene_source
	source/Components/CameraComponent.cpp
	source/Components/ControllerComponent.cpp
	source/Components/LightComponent.cpp
	source/Components/MeshComponent.cpp
	source/Components/TransformComponent.cpp
	source/Controllers/CameraController.cpp
	source/Scene/Camera.cpp
	source/Scene/GameObject.cpp
	source/Scene/Scene.cpp
	source/Scene/SceneManager.cpp
)

add_library (Scene SHARED ${ChaiScene_headers} ${ChaiScene_source})

target_include_directories(Scene PUBLIC include)
target_link_libraries(Scene PUBLIC Core ChaiGraphics)

source_group("Scene" REGULAR_EXPRESSION ".*/Scene/.*\\.(h|cpp)$")
source_group("Components" REGULAR_EXPRESSION ".*/Components/.*")
source_group("Controllers" REGULAR_EXPRESSION ".*/Controllers/.*")

ChaiLibDefaults(Scene "Scene")