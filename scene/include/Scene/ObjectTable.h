#pragma once
#include <unordered_map>
#include <cstdint>
#include <vector>

namespace chai::scene
{
	class Object;

	class ObjectTable
	{
    public:
        static ObjectTable& instance();

		//return a single int packed with id and generation
		uint64_t add(Object*);
        void remove(uint64_t id);
        Object* resolve(uint64_t id);

	private:
        ObjectTable() = default;

		struct Slot
		{
            Object* objPointer = nullptr;
            uint32_t generation = 0;
		};

        std::vector<Slot> objectSlots_;
        std::vector<uint32_t> freeSlots_;
	};

	std::pair<uint32_t, uint32_t> unpack(uint64_t);
    uint64_t pack(std::pair<uint32_t, uint32_t>);
}