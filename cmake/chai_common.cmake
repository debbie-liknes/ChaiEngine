function(ChaiLibDefaults target_name optional_path)
    #generate the shared lib header export
    include(GenerateExportHeader)
    generate_export_header(${target_name} EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/exports/${target_name}/${target_name}Export.h")

    #want to be able to include the directory via #include <MyLibraryExport.h>
    target_include_directories(${target_name} PUBLIC ${CMAKE_BINARY_DIR}/exports/${target_name})

    file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS include/*.h)

    foreach(file IN LISTS HEADER_FILES)
        # Get the path relative to the include dir
        file(RELATIVE_PATH relPath "${CMAKE_CURRENT_SOURCE_DIR}/include" "${file}")
    
        # Extract the directory part ("Path/Subdir")
        get_filename_component(group "${relPath}" DIRECTORY)

        # Replace slashes with backslashes for Visual Studio
        string(REPLACE "/" "\\" group "${group}")

        source_group("${group}" FILES "${file}")
    endforeach()

    # Then use HEADER_FILES in your target
    target_sources(${target_name} PRIVATE ${HEADER_FILES})

    #organize into folders in the IDE
    if (optional_path)
        set_target_properties(${target_name} PROPERTIES FOLDER ${optional_path})
    endif()
endfunction()

function(ChaiPluginDefaults target_name optional_path)
    ChaiLibDefaults(${target_name} ${optional_path})

    set_target_properties(${target_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/plugins")
endfunction()