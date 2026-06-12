project(VulkanRenderer)

set(${PROJECT_NAME}_headers
        core/VkCheck.h
        core/VkCheck.cpp
        pipeline/PipelineBuilder.h
        pipeline/ShaderModule.h
        rendering/VulkanRenderer.h
        VulkanContext.h
        RenderTargetView.h
        Swapchain.h
        Buffer.h
        Mesh.h
        GpuResources.h
        TextureFactory.h
        ImageTransition.h
        VkUtils.h
        Material.h
)

set(${PROJECT_NAME}_source
        pipeline/PipelineBuilder.cpp
        pipeline/ShaderModule.cpp
        rendering/VulkanPlugin.cpp
        rendering/VulkanRenderer.cpp
        Swapchain.cpp
        VulkanContext.cpp
        VulkanMemoryAllocator.cpp
        Buffer.cpp
        ImageTransition.cpp
)

include(FetchContent)

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

target_link_libraries(${PROJECT_NAME} PUBLIC Core Graphics Vulkan::Vulkan PRIVATE vk-bootstrap::vk-bootstrap GPUOpen::VulkanMemoryAllocator)
#set_target_properties(vkbootstrap PROPERTIES FOLDER "external")

ChaiPluginDefaults(${PROJECT_NAME} "Plugins")