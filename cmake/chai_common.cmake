function(chai_source_groups target)
    get_target_property(target_sources ${target} SOURCES)

    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${target_sources}
    )
endfunction()

function(generate_plugin_manifest target)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -D "SOURCE_DIR=${CMAKE_SOURCE_DIR}"
            -D "PLUGIN_NAME=$<TARGET_PROPERTY:${target},NAME>"
            -D "PLUGIN_AUTHOR=$<TARGET_PROPERTY:${target},CHAI_PLUGIN_AUTHOR>"
            -D "PLUGIN_VERSION=$<TARGET_PROPERTY:${target},CHAI_PLUGIN_VERSION>"
            -D "PLUGIN_RELOADABLE=$<TARGET_PROPERTY:${target},CHAI_PLUGIN_RELOADABLE>"
            -D "TARGET_NAME=$<TARGET_FILE_NAME:${target}>"
            -D "TARGET_FILE=$<TARGET_FILE:${target}>"
            -D "INPUT_FILE=${CMAKE_CURRENT_FUNCTION_LIST_DIR}/plugin_manifest.json.in"
            -D "OUTPUT_FILE=$<TARGET_FILE_DIR:${target}>/${target}.json"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/GeneratePluginManifest.cmake"
        COMMENT "Generating plugin manifest for $<TARGET_FILE_NAME:${target}> at $<TARGET_FILE_DIR:${target}>/${target}.json"
        VERBATIM
    )
endfunction()

function(ChaiLibDefaults target_name optional_path)
    #generate the shared lib header export
    include(GenerateExportHeader)
    generate_export_header(${target_name} EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/exports/${target_name}/${target_name}Export.h")

    #want to be able to include the directory via #include <MyLibraryExport.h>
    target_include_directories(${target_name} PUBLIC ${CMAKE_BINARY_DIR}/exports/${target_name})

    #organize into folders in the IDE
    if (optional_path)
        set_target_properties(${target_name} PROPERTIES FOLDER ${optional_path})
    endif ()

    # Edit and continue is disabled for tracy. See the tracy manual for more details.
    # set_target_properties(${target_name} PROPERTIES
    #     MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    # )

    # Link all targets to tracy for profiling capability
    target_link_libraries(${target_name} PUBLIC Tracy::TracyClient)

    chai_source_groups(${target_name})
endfunction()

function(ChaiPluginDefaults target_name optional_path)
    ChaiLibDefaults(${target_name} ${optional_path})

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"

        # Custom target properties for metadata
        CHAI_TARGET_TYPE "plugin"
        CHAI_PLUGIN_AUTHOR "chai"
        CHAI_PLUGIN_VERSION "0.0.1"
        CHAI_PLUGIN_RELOADABLE "false"
    )

    generate_plugin_manifest(${target_name})
endfunction()
