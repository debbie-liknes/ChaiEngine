project(VulkanRenderer)

set(${PROJECT_NAME}_headers
        commands/ImageTransition.h
        pipeline/PipelineBuilder.h
        pipeline/PipelineKey.h
        pipeline/PipelineRegistry.h
        pipeline/ShaderCompiler.h
        registries/ViewportRegistry.h
        renderer/VulkanRenderer.h
        renderer/VulkanContext.h

        renderGraph/ChaiRenderGraph.h
        renderGraph/CRGBuilder.h
        renderGraph/CRGDescriptors.h
        renderGraph/CRGPass.h
        renderGraph/CRGResources.h

        resources/RenderTargetView.h
        resources/Buffer.h
        resources/GpuResources.h
        resources/Material.h
        resources/MaterialFactory.h
        resources/MaterialRegistry.h
        resources/Mesh.h
        resources/MeshFactory.h
        resources/MeshRegistry.h
        resources/ModelRegistry.h
        resources/TextureFactory.h
        resources/TextureRegistry.h
        resources/UploadContext.h
        resources/VulkanTexture.h
        resources/VulkanVertex.h
        resources/VulkanRenderTarget.h
        resources/VulkanViewport.h
        swapchain/Swapchain.h
        utils/VkCheck.h
        utils/VkUtils.h
        utils/GpuProfiler.h

        VulkanPluginStatsPanel.h
)

set(${PROJECT_NAME}_source
        commands/ImageTransition.cpp
        pipeline/PipelineBuilder.cpp
        pipeline/PipelineRegistry.cpp
        pipeline/ShaderCompiler.cpp
        registries/ViewportRegistry.cpp

        renderGraph/ChaiRenderGraph.cpp
        renderGraph/CRGBuilder.cpp
        renderGraph/CRGResources.cpp

        renderer/VulkanContext.cpp
        renderer/VulkanRenderer.cpp
        resources/Buffer.cpp
        resources/MaterialFactory.cpp
        resources/MaterialRegistry.cpp
        resources/MeshFactory.cpp
        resources/MeshRegistry.cpp
        resources/ModelRegistry.cpp
        resources/UploadContext.cpp
        resources/TextureFactory.cpp
        resources/TextureRegistry.cpp
        resources/VulkanRenderTarget.cpp
        swapchain/Swapchain.cpp
        utils/VkCheck.cpp
        utils/GpuProfiler.cpp

        VulkanCommon.h
        VulkanMemoryAllocator.cpp
        VulkanPlugin.cpp
)

include(FetchContent)

set(VK_BOOTSTRAP_POSITION_INDEPENDENT_CODE ON)

FetchContent_Declare(
    vkbootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
    GIT_TAG        v1.4.352
)

FetchContent_MakeAvailable(vkbootstrap)

include(FetchContent)

FetchContent_Declare(
    VMA
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(VMA)


add_library(${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_headers}
        ${${PROJECT_NAME}_source}
)

find_package(Vulkan REQUIRED)

set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/shaders")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${SHADER_OUTPUT_DIR}
)

#unsure about this
set(CHAI_SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/ShaderPaths.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/ShaderPaths.h
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Runtime
    Graphics
    UICore
    Vulkan::Vulkan
    glfw
    PRIVATE
    Graph
    ImGui
    EditorUI
    vk-bootstrap::vk-bootstrap
    GPUOpen::VulkanMemoryAllocator
    unofficial::shaderc::shaderc
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
set_target_properties(vk-bootstrap PROPERTIES FOLDER "external")

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")
