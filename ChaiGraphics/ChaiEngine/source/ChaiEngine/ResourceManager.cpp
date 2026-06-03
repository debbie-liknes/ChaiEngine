#include <ChaiEngine/ResourceManager.h>

namespace chai
{
    ResourceManager& ResourceManager::instance()
    {
        static ResourceManager instance;
        return instance;
    }
} // namespace chai