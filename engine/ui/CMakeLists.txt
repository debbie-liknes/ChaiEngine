project(UI)

set(${PROJECT_NAME}_headers
    include/UI/Editor/DockspaceService.h
    include/UI/Editor/PanelHost.h
    include/UI/Editor/PanelRegistry.h
    include/UI/Tools/InternalChaiUi.h
    include/UI/Tools/FontManager.h
    include/UI/Tools/EditorViewportManager.h
)

set(${PROJECT_NAME}_source
	source/UI/Editor/DockspaceService.cpp
	source/UI/Editor/PanelHost.cpp
	source/UI/Editor/PanelRegistry.cpp
	source/UI/Tools/InternalChaiUi.cpp
	source/UI/Tools/FontManager.cpp
	source/UI/Tools/EditorViewportManager.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Logger Graphics PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Engine")