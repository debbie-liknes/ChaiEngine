#include <Scene/Object.h>
#include <Scene/ObjectTable.h>

namespace chai::scene
{
    Object::Object() : id_(ObjectTable::instance().add(this))
    {

    }

    ObjectId Object::id() const
    {
        return id_;
    }
}