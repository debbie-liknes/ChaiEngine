# Add source to this project's executable.

set(Core_headers
	include/Core/Containers.h
	include/Core/MemoryTypes.h
)

set(Core_source
	source/Core/Containers.cpp
)

add_library(Core SHARED ${Core_headers} ${Core_source})

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)

ChaiLibDefaults(Core "Core")