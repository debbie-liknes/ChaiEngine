# CMakeList.txt : CMake project for Chai, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (VulkanRenderer "include/VulkanApi.h")
set_target_properties(VulkanRenderer PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VulkanRenderer PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(VulkanRenderer ChaiGraphics)