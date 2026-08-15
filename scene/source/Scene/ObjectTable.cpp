#include <Scene/Object.h>
#include <Scene/ObjectTable.h>

namespace chai::scene
{
    ObjectTable& ObjectTable::instance()
    {
        static ObjectTable instance;
        return instance;
    }

    uint64_t ObjectTable::add(Object* objToadd)
    {
        // find if theres a free slot to reuse
        uint32_t idx;
        if (!freeSlots_.empty()) {
            idx = freeSlots_.back();
            freeSlots_.pop_back();
            objectSlots_[idx].objPointer = objToadd;
        } else {
            idx = objectSlots_.size();
            objectSlots_.emplace_back(objToadd, 1);
        }

        // return packed
        return pack({idx, objectSlots_[idx].generation});
    }

    void ObjectTable::remove(uint64_t id)
    {
        // find it in the slot and match index + generation
        const auto [idx, gen] = unpack(id);
        if (idx >= objectSlots_.size() || objectSlots_[idx].generation != gen)
            return;
        objectSlots_[idx].generation++;         //invalidate the generation
        objectSlots_[idx].objPointer = nullptr;
        freeSlots_.push_back(idx);
    }

    Object* ObjectTable::resolve(uint64_t id)
    {
        const auto [idx, gen] = unpack(id);
        if (idx >= objectSlots_.size() || objectSlots_[idx].generation != gen)
            return nullptr;
        return objectSlots_[idx].objPointer;
    }

    std::pair<uint32_t, uint32_t> unpack(uint64_t idToUnpack)
    {
        return {static_cast<uint32_t>(idToUnpack >> 32), static_cast<uint32_t>(idToUnpack)};
    }

    uint64_t pack(std::pair<uint32_t, uint32_t> ids)
    {
        return (uint64_t{ids.first} << 32) | ids.second;
    }
} // namespace chai::scene