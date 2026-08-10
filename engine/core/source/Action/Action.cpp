#include <Action/Action.h>

namespace chai
{
    void Action::trigger() const
    {
        if (registeredAction_)
            registeredAction_.operator()();
    }
}
