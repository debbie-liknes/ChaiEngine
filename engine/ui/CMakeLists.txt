project(UI)

set(${PROJECT_NAME}_headers
    include/UI/Editor/DockspaceService.h
    include/UI/Editor/PanelHost.h
    include/UI/Editor/PanelRegistry.h
    include/UI/Editor/InternalChaiUI.h
    include/UI/Editor/FontManager.h
    include/UI/Editor/EditorViewportManager.h
    include/UI/Editor/MenuService.h
    include/UI/Editor/TreeNode.h
    include/UI/Editor/IconRegistry.h
)

set(${PROJECT_NAME}_source
	source/UI/Editor/DockspaceService.cpp
	source/UI/Editor/PanelHost.cpp
	source/UI/Editor/PanelRegistry.cpp
	source/UI/Editor/InternalChaiUI.cpp
	source/UI/Editor/FontManager.cpp
	source/UI/Editor/EditorViewportManager.cpp
	source/UI/Editor/MenuService.cpp
	source/UI/Editor/TreeNode.cpp
	source/UI/Editor/IconRegistry.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Logger Graphics TypeReflection PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
