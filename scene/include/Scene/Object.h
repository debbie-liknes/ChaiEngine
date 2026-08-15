#pragma once
#include <cstdint>

namespace chai::scene
{
	using ObjectId = uint64_t;

	class Object
	{
    public:
        Object();

		ObjectId id() const;

	private:
        ObjectId id_;
	};
}