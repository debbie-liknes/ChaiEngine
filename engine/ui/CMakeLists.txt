project(UI)

set(${PROJECT_NAME}_headers
    include/UI/InternalChaiUI.h
    include/UI/FontManager.h
    include/UI/TreeNode.h
)

set(${PROJECT_NAME}_source
	source/UI/InternalChaiUI.cpp
	source/UI/FontManager.cpp
	source/UI/TreeNode.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Containers Core Logger Graphics Window PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
