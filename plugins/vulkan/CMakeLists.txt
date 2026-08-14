project(VulkanRenderer)

set(${PROJECT_NAME}_headers
        commands/ImageTransition.h
        pipeline/PipelineBuilder.h
        pipeline/PipelineCache.h
        pipeline/PipelineHelpers.h
        pipeline/PipelineKey.h
        pipeline/ShaderModule.h
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
        pipeline/PipelineCache.cpp
        pipeline/PipelineHelpers.cpp
        pipeline/ShaderModule.cpp
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

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/shaders")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.comp"
    "${SHADER_SOURCE_DIR}/*.geom"
    "${SHADER_SOURCE_DIR}/*.tesc"
    "${SHADER_SOURCE_DIR}/*.tese"
)

set(SPIRV_FILES)

foreach(SHADER ${SHADER_FILES})

    file(RELATIVE_PATH REL_PATH
        "${SHADER_SOURCE_DIR}"
        "${SHADER}")

    set(SPIRV "${SHADER_OUTPUT_DIR}/${REL_PATH}.spv")

    get_filename_component(SPIRV_DIR "${SPIRV}" DIRECTORY)

    add_custom_command(
        OUTPUT "${SPIRV}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SPIRV_DIR}"
        COMMAND ${Vulkan_GLSLC_EXECUTABLE}
                "${SHADER}"
                -o "${SPIRV}"
        DEPENDS "${SHADER}"
        VERBATIM
    )

    list(APPEND SPIRV_FILES "${SPIRV}")

endforeach()

add_custom_target(Shaders ALL
    DEPENDS ${SPIRV_FILES}
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
)
set_target_properties(vk-bootstrap PROPERTIES FOLDER "external")

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")
