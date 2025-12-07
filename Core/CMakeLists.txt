# Add source to this project's executable.

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/)

set(Core_headers
        include/Asset/AssetHandle.h
        include/Asset/AssetLoader.h
        include/Asset/AssetManager.h
        include/Core/Updatable.h
        include/Graphics/Viewport.h
        include/Input/EventHandler.h
        include/Input/InputState.h
        include/Input/InputSystem.h
        include/Meta/ChaiMacros.h
        include/Meta/MethodInvoker.h
        include/Meta/TypeInfo.h
        include/Meta/TypeRegistry.h
        include/Plugin/PluginBase.h
        include/Plugin/PluginRegistry.h
        include/Plugin/ServiceLocator.h
        include/Plugin/ServiceRegistry.h
        include/Resource/ResourceManager.h
        include/Resource/ResourcePool.h
        include/Resource/Resource.h
        include/Scene/ICamera.h
        include/Types/CMap.h
)

set(Core_source
        source/Asset/AssetManager.cpp
        source/Core/Updatable.cpp
        source/Graphics/Viewport.cpp
        source/Input/InputState.cpp
        source/Input/InputSystem.cpp
        source/Meta/TypeRegistry.cpp
        source/Plugin/PluginRegistry.cpp
        source/Plugin/ServiceLocator.cpp
        source/Resource/ResourceManager.cpp
        source/Scene/ICamera.cpp
        include/Graphics/AABB.h
        include/Graphics/AABB.cpp
        include/Graphics/Frustum.cpp
        include/Graphics/Frustum.h
)

add_library(Core SHARED
        ${Core_headers}
        ${Core_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC ChaiMath)
target_compile_definitions(Core PUBLIC RESOURCE_PATH=\"${RESOURCE_DIR}\")

#meh
source_group("Core" REGULAR_EXPRESSION ".*/Core/.*\\.(h|cpp)$")
source_group("Input" REGULAR_EXPRESSION ".*/Input/.*")
source_group("Graphics" REGULAR_EXPRESSION ".*/Graphics/.*")
source_group("Meta" REGULAR_EXPRESSION ".*/Meta/.*")
source_group("Scene" REGULAR_EXPRESSION ".*/Scene/.*")
source_group("Resource" REGULAR_EXPRESSION ".*/Resource/.*")
source_group("Plugin" REGULAR_EXPRESSION ".*/Plugin/.*")
source_group("Types" REGULAR_EXPRESSION ".*/Types/.*")


ChaiLibDefaults(Core "Core")