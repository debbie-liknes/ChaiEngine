function(ChaiLibDefaults target_name optional_path)
    #generate the shared lib header export
    include(GenerateExportHeader)
    generate_export_header(${target_name} EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/exports/${target_name}/${target_name}Export.h")

    #linker stuff
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

    if (CMAKE_VERSION VERSION_GREATER 3.12)
        #(TARGET ${target_name} PROPERTY CXX_STANDARD 20)
    endif()

    #want to be able to include the directory via #include <MyLibraryExport.h>
    target_include_directories(${target_name} PUBLIC ${CMAKE_BINARY_DIR}/exports/${target_name})

    #organize into folders in the IDE
    if (optional_path)
        set_target_properties(${target_name} PROPERTIES FOLDER ${optional_path})
    endif()
endfunction()