
set(ChaiEngine_headers
	include/Engine/Engine.h
)

set(ChaiEngine_source
	source/Engine/Engine.cpp
)

add_library (ChaiEngine SHARED ${ChaiEngine_headers} ${ChaiEngine_source})

target_link_libraries(ChaiEngine PUBLIC ChaiGraphics Scene WindowModule)
target_include_directories(ChaiEngine PUBLIC include)

ChaiLibDefaults(ChaiEngine "Engine")