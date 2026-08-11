#include <Core/Action.h>

namespace chai
{
    void Action::trigger() const
    {
        if (callback_)
            callback_.operator()();
    }
}
