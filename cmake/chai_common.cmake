function(chai_source_groups target)
    get_target_property(target_sources ${target} SOURCES)

    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${target_sources}
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

    chai_source_groups(${target_name})
endfunction()

function(ChaiPluginDefaults target_name optional_path)
    ChaiLibDefaults(${target_name} ${optional_path})

    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins")
endfunction()
