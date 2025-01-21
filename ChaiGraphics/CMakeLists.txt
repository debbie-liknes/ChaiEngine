project(ChaiGraphics)

# Add source to this project's executable.
add_library (${PROJECT_NAME} SHARED "include/IGraphicsApi.h" "source/IGraphicsApi.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC include)

ChaiLibDefaults(${PROJECT_NAME} "Graphics")