# CMakeList.txt : CMake project for Chai, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (OpenGLRenderer "include/OpenGLApi.h")
set_target_properties(OpenGLRenderer PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OpenGLRenderer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.