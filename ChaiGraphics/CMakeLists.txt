# CMakeList.txt : CMake project for Chai, include source and define
# project specific logic here.
#
project(ChaiGraphics)
# Add source to this project's executable.
add_library (ChaiGraphics SHARED "include/IGraphicsApi.h" "source/IGraphicsApi.cpp")
set_target_properties(ChaiGraphics PROPERTIES LINKER_LANGUAGE CXX)


target_compile_definitions(ChaiGraphics PRIVATE BUILD_CHAIGRAPHICS)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ChaiGraphics PROPERTY CXX_STANDARD 20)
endif()

#target_include_directories(ChaiGraphics PUBLIC include)