
add_library (VulkanRenderer "include/VulkanApi.h")

find_package(vulkan REQUIRED)

ChaiLibDefaults(VulkanRenderer "Graphics/Platforms")