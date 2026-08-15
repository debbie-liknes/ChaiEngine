cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake/share"
)
include(${CMAKE_SOURCE_DIR}/cmake/chai_common.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# When exporting STL types, MSVC will generate a warning saying that STL types
# are not accessible by clients. This disables that warning.
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IGNORE:4197")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /IGNORE:4197")
    add_compile_options(/wd4251)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set up vcpkg to work with find_package calls
set(VCPKG_INSTALL_OPTIONS "--no-print-usage")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Chai)

# Enable testing via CTest
enable_testing()

# Include external projects
add_subdirectory(external)

# Add include-only headers
include_directories(external/include)

find_package(unofficial-shaderc CONFIG REQUIRED)

# Include sub-projects.
add_subdirectory(base)
add_subdirectory(editor)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(platform)
add_subdirectory(plugins)
add_subdirectory(scene)
add_subdirectory(ui)
