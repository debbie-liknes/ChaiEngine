project(EditorUI)

set(${PROJECT_NAME}_headers
    include/EditorUI/SceneHierarchy.h
    include/EditorUI/ActionManager.h
    include/EditorUI/CommandPalette.h
    include/EditorUI/DockspaceService.h
    include/EditorUI/PanelHost.h
    include/EditorUI/PanelRegistry.h
    include/EditorUI/PluginManager.h
    include/EditorUI/PropertiesPane.h
    include/EditorUI/EditorMenu.h
    include/EditorUI/EditorViewportManager.h
    include/EditorUI/ActionConfigLoader.h
)

set(${PROJECT_NAME}_source
	source/EditorUI/SceneHierarchy.cpp
	source/EditorUI/ActionManager.cpp
	source/EditorUI/CommandPalette.cpp
	source/EditorUI/DockspaceService.cpp
	source/EditorUI/PanelHost.cpp
	source/EditorUI/PanelRegistry.cpp
	source/EditorUI/PluginManager.cpp
	source/EditorUI/PropertiesPane.cpp
	source/EditorUI/EditorMenu.cpp
	source/EditorUI/EditorViewportManager.cpp
	source/EditorUI/ActionConfigLoader.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Containers Core Logger Graphics Window Scene UICore Runtime PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Editor")
