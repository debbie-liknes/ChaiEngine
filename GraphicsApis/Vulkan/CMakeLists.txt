project(VulkanRenderer)

add_library (${PROJECT_NAME} "include/VulkanApi.h")

ChaiLibDefaults(${PROJECT_NAME} "Graphics/Platforms")