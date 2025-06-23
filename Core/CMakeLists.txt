# Add source to this project's executable.

#this cmake file needs help
set(Core_headers
	include/Core/TypeHelpers.h
	include/Core/TextureBackend.h

	include/Meta/TypeInfo.h
	include/Meta/TypeRegistry.h
	include/Meta/ChaiMacros.h
	include/Meta/MethodInvoker.h

	include/Scene/ICamera.h
	include/Graphics/IViewport.h
)

set(Core_source
	source/Meta/TypeRegistry.cpp

	source/Scene/ICamera.cpp
	source/Graphics/IViewport.cpp
)

set(Plugin_headers
	include/Plugin/PluginBase.h
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

set(RESOURCE_DIR ${CMAKE_SOURCE_DIR}/)

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
target_compile_definitions(Core PUBLIC RESOURCE_PATH=\"${RESOURCE_DIR}\")


ChaiLibDefaults(Core "Core")