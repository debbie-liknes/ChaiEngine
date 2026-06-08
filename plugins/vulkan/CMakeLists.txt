project(VulkanRenderer)

set(${PROJECT_NAME}_headers
        VulkanRenderer.h
        RenderTargetView.h
        Swapchain.h
)

set(${PROJECT_NAME}_source
        VulkanPlugin.cpp
        VulkanRenderer.cpp
)

add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC Core Graphics Vulkan::Vulkan)

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")