project(VulkanRenderer)

set(${PROJECT_NAME}_headers
        core/VkCheck.h
        core/VkCheck.cpp
        rendering/VulkanRenderer.h
        VulkanContext.h
        RenderTargetView.h
        Swapchain.h
)

set(${PROJECT_NAME}_source
        rendering/VulkanPlugin.cpp
        rendering/VulkanRenderer.cpp
        Swapchain.cpp
        VulkanContext.cpp
)

include(FetchContent)

FetchContent_Declare(
    vkbootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG        v1.4.352
)

FetchContent_MakeAvailable(vkbootstrap)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC Core Graphics Vulkan::Vulkan PRIVATE vk-bootstrap::vk-bootstrap)

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")