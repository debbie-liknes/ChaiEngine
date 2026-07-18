project(UI)

set(${PROJECT_NAME}_headers
        include/UI/Tools/InternalChaiUi.h
        include/UI/Tools/InternalPanels.h
        include/UI/Tools/FontManager.h
)

set(${PROJECT_NAME}_source
	source/UI/Tools/InternalChaiUi.cpp
	source/UI/Tools/InternalPanels.cpp
	source/UI/Tools/FontManager.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath Logger PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Engine")