
set(ChaiApi_headers
	include/Engine/IRenderObject.h
	include/Engine/IViewer.h
	include/Engine/IScene.h
	include/Engine/IPipeline.h
	include/Engine/PipelineState.h
	include/Engine/VertexBuffer.h
	include/Engine/UniformBuffer.h
	include/Engine/Shader.h
)

set(ChaiApi_source
	source/Engine/VertexBuffer.cpp
	source/Engine/UniformBuffer.cpp
)

find_package(glm CONFIG REQUIRED)

add_library(ChaiApi SHARED ${ChaiApi_headers} ${ChaiApi_source})

target_include_directories(ChaiApi PUBLIC include)
target_link_libraries(ChaiApi PUBLIC Core glm::glm WindowModule)

ChaiLibDefaults(ChaiApi "Graphics")