#include <Resource/ResourceManager.h>

namespace chai
{
    ResourceManager& ResourceManager::instance()
    {
        static ResourceManager instance;
        return instance;
    }
}