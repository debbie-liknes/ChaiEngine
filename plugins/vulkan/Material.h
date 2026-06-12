#pragma once
#include <vulkan/vulkan.h>

namespace chai::gfx
{
	struct Material
	{
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout layout = VK_NULL_HANDLE;
	};
}