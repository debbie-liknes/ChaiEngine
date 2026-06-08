project(Logger)

set(${PROJECT_NAME}_headers
        include/Log.h
        include/LogLevel.h
        include/LogSink.h
)

set(${PROJECT_NAME}_source
        source/Log.cpp
)

add_library(${PROJECT_NAME} STATIC
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

ChaiLibDefaults(${PROJECT_NAME} "Engine")