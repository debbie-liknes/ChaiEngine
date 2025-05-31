cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(${CMAKE_SOURCE_DIR}/cmake/chai_common.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#idc about abi compatibility rn
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /IGNORE:4197")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /IGNORE:4197")
add_compile_options(/wd4251)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

project ("Chai")

# Include external projects
add_subdirectory(external)

# Add include-only headers
include_directories(external/include)

# Include sub-projects.
add_subdirectory ("Core")
add_subdirectory ("ChaiApps")
add_subdirectory ("ChaiGraphics")
add_subdirectory ("GraphicsApis")
add_subdirectory ("Engine")
add_subdirectory ("Window")
add_subdirectory ("Scene")
add_subdirectory ("Plugins")
