project(Editor)

set(${PROJECT_NAME}_headers
    include/Runtime/Editor.h
    include/Runtime/EditorSelection.h
)

set(${PROJECT_NAME}_source
    source/Runtime/Editor.cpp
    source/Runtime/EditorSelection.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Runtime 
    SpdLogger
    EditorUI
    DiagnosticsUI
    Graphics
    Scene
    Settings
    Audio
    OS
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    CHAI_ASSET_DIR="${CMAKE_SOURCE_DIR}/assets")

ChaiLibDefaults(${PROJECT_NAME} "Editor")
