project(OS)

set(${PROJECT_NAME}_headers
    include/OS/Clock.h
    include/OS/DynamicLibrary.h
    include/OS/FileIO.h
    include/OS/Process.h
    include/OS/SystemPaths.h
)

set(${PROJECT_NAME}_source
    source/OS/DynamicLibrary.cpp
    source/OS/FileIO.cpp
    source/OS/Process.cpp
    source/OS/SystemPaths.cpp
)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_headers}
    ${${PROJECT_NAME}_source}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Logger)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Base")
