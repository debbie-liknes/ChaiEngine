
set(VulkanRenderer_headers
        VulkanRenderer.h
)

set(VulkanRenderer_source
        VulkanRenderer.cpp
)

find_package(Vulkan REQUIRED)

add_library(VulkanRenderer SHARED ${VulkanRenderer_headers} ${VulkanRenderer_source})

target_link_libraries(VulkanRenderer PUBLIC Vulkan::Vulkan ChaiGraphics WindowModule ChaiMath)
#target_include_directories(VulkanRenderer PUBLIC include)

target_compile_definitions(VulkanRenderer PRIVATE
        CMAKE_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}/\"
)

ChaiPluginDefaults(VulkanRenderer "Graphics/Platforms")