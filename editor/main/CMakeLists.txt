project(ChaiEditor)

add_executable(${PROJECT_NAME} "ChaiEditor.cpp")

#just use the OpenGL Renderer for now
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    Editor
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=$ENV{FMOD_HOME}/api/core/lib/x64;%PATH%")
endif()

#copy game assets
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/assets/game
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/game)


# only for dev/editor builds
#if(CHAI_BUILD_EDITOR_TOOLS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/editor
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/editor)
#endif()