project(UICore)

set(${PROJECT_NAME}_headers
    include/UI/Core/InternalChaiUI.h
    include/UI/Core/FontManager.h
    include/UI/Core/TreeNode.h
)

set(${PROJECT_NAME}_source
	source/UI/Core/InternalChaiUI.cpp
	source/UI/Core/FontManager.cpp
	source/UI/Core/TreeNode.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath PRIVATE ImGui Logger)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
