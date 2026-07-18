project(UI)

set(${PROJECT_NAME}_headers
        include/UI/Tools/InternalChaiUi.h
        include/UI/Tools/InternalPanels.h
)

set(${PROJECT_NAME}_source
	source/UI/Tools/InternalChaiUi.cpp
	source/UI/Tools/InternalPanels.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath PRIVATE ImGui)

ChaiLibDefaults(${PROJECT_NAME} "Engine")