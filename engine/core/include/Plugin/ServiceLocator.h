/**
 * @file ServiceLocator.h
 * @brief Defines the ServiceLocator class, which provides a simple way for plugins to register and
 * resolve services by type.
 */
#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <cassert>

namespace chai
{
    /**
     * @brief A simple service locator that allows plugins to register and resolve services by type.
     * Services are stored as shared_ptr<void> internally, and resolved by casting to the requested
     * type.
     */
    class ServiceLocator
    {
    public:
        template <typename Interface>
        void provide(std::shared_ptr<Interface> impl)
        {
            services_[std::type_index(typeid(Interface))] = std::move(impl);
        }

        template <typename Interface>
        std::shared_ptr<Interface> tryResolve() const
        {
            auto it = services_.find(std::type_index(typeid(Interface)));
            return it == services_.end() ? nullptr
                                         : std::static_pointer_cast<Interface>(it->second);
        }

        template <typename Interface>
        Interface& resolve() const
        { 
            // asserting variant
            auto p = tryResolve<Interface>();
            assert(p && "service not registered");
            return *p;
        }

        template <typename Interface>
        void remove()
        {
            services_.erase(std::type_index(typeid(Interface)));
        }

    private:
        std::unordered_map<std::type_index, std::shared_ptr<void>> services_;
    };
}