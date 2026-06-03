project(Logger)

set(${PROJECT_NAME}_headers
        include/Log.h
        include/LogLevel.h
        include/LogSink.h
        include/SpdLogSink.h
)

set(${PROJECT_NAME}_source
        source/Log.cpp
        source/SpdLogSink.cpp
)

add_library(${PROJECT_NAME}
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

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
#add_subdirectory(tests)

ChaiLibDefaults(${PROJECT_NAME} "Engine")