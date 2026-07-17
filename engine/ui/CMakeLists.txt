project(UI)

set(${PROJECT_NAME}_headers
        include/UI/Tools/InternalChaiUi.h
)

set(${PROJECT_NAME}_source
	source/UI/Tools/InternalChaiUi.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/exports/Core include)
target_link_libraries(${PROJECT_NAME} PUBLIC TypeReflection Core)

ChaiLibDefaults(${PROJECT_NAME} "Engine")