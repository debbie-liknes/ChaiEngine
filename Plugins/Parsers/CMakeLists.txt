# Add source to this project's executable.

set(Parser_headers
	include/Parsers/JsonParser.h
)

set(Parser_source
	source/Parsers/JsonParser.cpp
)

add_library(Parsers SHARED 
	${Parser_headers} 
	${Parser_source}
)

target_include_directories(Parsers PUBLIC include ${CMAKE_SOURCE_DIR}/external/nlohmann)
target_link_libraries(Parsers PRIVATE Core)

ChaiPluginDefaults(Parsers "Core/Plugins")