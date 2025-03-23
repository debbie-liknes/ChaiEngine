# Add source to this project's executable.

set(Core_headers
	include/Core/Containers.h
	include/Core/MemoryTypes.h
	include/Core/TypeHelpers.h
)

set(Core_source
	source/Core/Containers.cpp
)

find_package(glm CONFIG REQUIRED)

add_library(Core SHARED ${Core_headers} ${Core_source})

target_include_directories(Core PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(Core PUBLIC glm::glm)

ChaiLibDefaults(Core "Core")