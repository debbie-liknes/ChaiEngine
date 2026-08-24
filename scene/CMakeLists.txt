# Add source to this project's executable.

set(ChaiScene_headers
        include/Components/Component.h
        include/Components/CameraComponent.h
        include/Components/ControllerComponent.h
        include/Components/LightComponent.h
        include/Components/MeshComponent.h
        include/Components/SkyboxComponent.h
        include/Components/TransformComponent.h
        include/Controllers/Controller.h
        include/Controllers/FlyCamController.h
        include/Scene/Camera.h
        include/Scene/GameObject.h
        include/Scene/Object.h
        include/Scene/ObjectTable.h
        include/Scene/Scene.h
        include/Scene/SpawnPrefab.h
        include/Controllers/SpinController.h
        include/Visitors/Visitor.h
        include/Visitors/AudioSceneVisitor.h
        include/Visitors/FrameRenderVisitor.h
        include/Visitors/SceneSaveVisitor.h
)

set(ChaiScene_source
        source/Components/Component.cpp
        source/Components/CameraComponent.cpp
        source/Components/ControllerComponent.cpp
        source/Components/LightComponent.cpp
        source/Components/MeshComponent.cpp
        source/Components/SkyboxComponent.cpp
        source/Components/TransformComponent.cpp
        source/Controllers/SpinController.cpp
        source/Controllers/FlyCamController.cpp
        source/Scene/Camera.cpp
        source/Scene/GameObject.cpp
        source/Scene/Object.cpp
        source/Scene/ObjectTable.cpp
        source/Scene/Scene.cpp
        source/Scene/SpawnPrefab.cpp
        source/Visitors/AudioSceneVisitor.cpp
        source/Visitors/FrameRenderVisitor.cpp
        source/Visitors/SceneSaveVisitor.cpp
)

add_subdirectory(tests)

add_library(Scene SHARED ${ChaiScene_headers} ${ChaiScene_source})

target_include_directories(Scene PUBLIC include)
target_link_libraries(Scene PUBLIC Audio Graphics TypeReflection UICore Window)

ChaiLibDefaults(Scene "Scene")
