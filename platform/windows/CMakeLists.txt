project(Windows)

set(${PROJECT_NAME}_headers
    include/Platform/Windows/ProcessWindows.h
    include/Platform/Windows/IPCMessageQueueWindows.h
)

set(${PROJECT_NAME}_source
    source/Platform/Windows/ProcessWindows.cpp
    source/Platform/Windows/IPCMessageQueueWindows.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_link_libraries(${PROJECT_NAME} PUBLIC Containers Logger OS)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Platform")

