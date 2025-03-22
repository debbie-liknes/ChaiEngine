#pragma once
#include <CoreExport.h>
//template meta programming to determine the underlying struct types?
//so creating vbos is easier

namespace chai
{
	enum class CORE_EXPORT PrimDataType
	{
		FLOAT,
		INT,
		UNSIGNED_INT
	};

	enum class CORE_EXPORT DataType
	{
		FLOAT,
		INT,
		UINT,
		BOOL,

		FLOAT_2,
		FLOAT_3,
		FLOAT_4,
		INT_2,
		INT_3,
		INT_4,
		UINT_2,
		UINT_3,
		UINT_4,
		BOOL_2,
		BOOL_3,
		BOOL_4,

		MAT_2,
		MAT_3,
		MAT_4,
		MAT_2X3,
		MAT_3X2,
		MAT_2X4,
		MAT_4X2,
		MAT_3X4,
		MAT_4X3,

		STRUCT
	};
}