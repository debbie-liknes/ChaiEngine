#pragma once
#include <CoreExport.h>
#include<memory>

namespace chai
{
	template <typename T>
	using CSharedPtr = std::shared_ptr<T>;

	template <typename T>
	using CUniquePtr = std::unique_ptr<T>;
}
