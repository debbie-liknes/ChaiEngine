#I am intentionally keeping this separate from the Logger project
#The choice of Log Sink should belong to the application
project(SpdLogger)

set(${PROJECT_NAME}_headers
        include/SpdLogSink.h
)

set(${PROJECT_NAME}_source
        source/SpdLogSink.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "external")

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC Logger PRIVATE spdlog::spdlog)

ChaiLibDefaults(${PROJECT_NAME} "Engine")