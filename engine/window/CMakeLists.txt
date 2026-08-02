project(Window)

set(${PROJECT_NAME}_headers
    include/Input/IInput.h
    include/Window/Window.h
    include/Window/WindowTypes.h
    include/Window/WindowTypes.h
)

set(${PROJECT_NAME}_source
    source/stub.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC ChaiMath)

ChaiLibDefaults(${PROJECT_NAME} "Engine")
