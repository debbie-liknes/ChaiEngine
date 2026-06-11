#pragma once
#include <cstdint>
#include <GraphicsEnums.h>

namespace chai::gfx
{
	struct BufferDescriptor
	{
        std::uint64_t size;
	};

	struct BindingDescription
	{
        std::uint32_t binding = 0;
        BindingType type = BindingType::UniformBuffer;
	};
}