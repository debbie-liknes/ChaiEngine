#pragma once
#include <Asset/AssetHandle.h>

namespace chai
{
	class Resource
	{
	public:
		explicit Resource(Handle assetHandle) {}
		Resource() = default;
		virtual ~Resource() = default;
	};
}