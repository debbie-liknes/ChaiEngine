# Add source to this project's executable.

set(ChaiGraphics_headers
	include/ChaiGraphics/IGraphicsApi.h
)

set(ChaiGraphics_source
	source/ChaiGraphics/IGraphicsApi.cpp
)

add_library (ChaiGraphics SHARED ${ChaiGraphics_headers} ${ChaiGraphics_source})

target_include_directories(ChaiGraphics PUBLIC include)

ChaiLibDefaults(ChaiGraphics "Graphics")