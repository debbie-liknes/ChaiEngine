project(Graphics)

set(${PROJECT_NAME}_headers
        include/Renderer.h
)

set(${PROJECT_NAME}_source
)

add_library(${PROJECT_NAME} INTERFACE
        ${${PROJECT_NAME}_headers}
        #${${PROJECT_NAME}_source}
)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_link_libraries(${PROJECT_NAME} PUBLIC)

#ChaiLibDefaults(${PROJECT_NAME} "Engine")
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")