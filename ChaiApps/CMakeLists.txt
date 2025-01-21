# CMakeList.txt : CMake project for Chai, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Chai "Chai.cpp" "Chai.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chai PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Chai ChaiGraphics)
