#pragma once
#include <Asset/AssetHandle.h>

namespace chai
{
	class Resource
	{
	public:
		explicit Resource(Handle assetHandle) {}
		virtual ~Resource() = default;
	};
}