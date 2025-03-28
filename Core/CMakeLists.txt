# Add source to this project's executable.

set(Core_headers
	include/Core/Containers.h
	include/Core/MemoryTypes.h
	include/Core/TypeHelpers.h
	include/Meta/TypeInfo.h
	include/Meta/TypeRegistry.h
	include/Meta/ChaiMacros.h
	include/Meta/MethodInvoker.h
)

set(Core_source
	source/Core/Containers.cpp
)

set(Plugin_headers
	include/Plugin/PluginBase.h
	include/Plugin/PluginLoader.h
	include/Plugin/PluginManifest.h
	include/Plugin/PluginRegistry.h
)

set(Plugin_source
	source/Plugin/PluginLoader.cpp
	source/Plugin/PluginRegistry.cpp
)

source_group("Core/headers" FILES ${Core_headers})
source_group("Core/src" FILES ${Core_source})

source_group("Plugin/headers" FILES ${Plugin_headers})
source_group("Plugin/src" FILES ${Plugin_source})

find_package(glm CONFIG REQUIRED)

add_library(Core SHARED 
	${Core_headers} 
	${Core_source}
	${Plugin_headers}
	${Plugin_source}
)

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC glm::glm)

ChaiLibDefaults(Core "Core")