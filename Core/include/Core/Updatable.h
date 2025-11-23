#pragma once
#include <CoreExport.h>

namespace chai
{
    class CORE_EXPORT IUpdatable
    {
    public:
        IUpdatable() = default;
        virtual ~IUpdatable() = default;
        virtual void update(double deltaTime) = 0;
    };
}