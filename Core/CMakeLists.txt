# Add source to this project's executable.

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