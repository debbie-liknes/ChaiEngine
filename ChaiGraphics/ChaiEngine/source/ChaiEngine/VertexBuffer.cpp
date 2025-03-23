#include <ChaiEngine/VertexBuffer.h>

namespace CGraphics
{
	SharedVBO<uint32_t> createIndexBuffer()
	{
		return std::make_shared<VBO<uint32_t>>(PrimDataType::UNSIGNED_INT, 1);
	}
}