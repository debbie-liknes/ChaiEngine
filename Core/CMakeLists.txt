# Add source to this project's executable.

set(Core_headers
	include/Core/Containers.h
	include/Core/MemoryTypes.h
	include/Core/Rect.h
	include/Core/TypeHelpers.h
	include/Core/TextureBackend.h
	include/Meta/TypeInfo.h
	include/Meta/TypeRegistry.h
	include/Meta/ChaiMacros.h
	include/Meta/MethodInvoker.h
)

set(Core_source
	source/Core/Containers.cpp
	source/Meta/TypeRegistry.cpp
)

set(Plugin_headers
	include/Plugin/PluginBase.h
	include/Plugin/PluginManifest.h
	include/Plugin/PluginRegistry.h
	include/Plugin/ServiceLocator.h
	include/Plugin/ServiceRegistry.h
)

set(Plugin_source
	source/Plugin/PluginRegistry.cpp
	source/Plugin/ServiceLocator.cpp
)

set(Resource_headers
	include/Resource/ResourceLoader.h
	include/Resource/ResourceManager.h
)

set(Resource_source
	source/Resource/ResourceManager.cpp
)

set(Types_headers
	include/Types/Texture2D.h
)

set(Types_source
	source/Types/Texture2D.cpp
)

source_group("Core/headers" FILES ${Core_headers})
source_group("Core/src" FILES ${Core_source})

source_group("Plugin/headers" FILES ${Plugin_headers})
source_group("Plugin/src" FILES ${Plugin_source})

source_group("Resource/headers" FILES ${Resource_headers})
source_group("Resource/src" FILES ${Resource_source})

source_group("Types/headers" FILES ${Types_headers})
source_group("Types/src" FILES ${Types_source})

add_library(Core SHARED 
	${Core_headers} 
	${Core_source}
	${Plugin_headers}
	${Plugin_source}
	${Resource_headers}
	${Resource_source}
	${Types_headers}
	${Types_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC glm::glm)

ChaiLibDefaults(Core "Core")