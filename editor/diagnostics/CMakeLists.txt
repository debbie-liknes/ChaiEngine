project(DiagnosticsUI)

set(${PROJECT_NAME}_headers
    include/DiagnosticsUI/LogPanel.h
)

set(${PROJECT_NAME}_source
	source/DiagnosticsUI/LogPanel.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC UI Logger PRIVATE ImGui spdlog::spdlog)

ChaiLibDefaults(${PROJECT_NAME} "Editor")