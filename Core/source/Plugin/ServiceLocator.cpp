#include <Plugin/ServiceLocator.h>

namespace chai
{
    ServiceLocator& ServiceLocator::instance()
    {
        static ServiceLocator locator;
        return locator;
    }
}