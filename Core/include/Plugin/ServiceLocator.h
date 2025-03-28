#pragma once
#include <memory>
#include <unordered_map>

namespace chai
{
    class ServiceLocator {
    public:
        static ServiceLocator& getInstance();

        void Shutdown();

        template<typename T>
        void Register(std::shared_ptr<T> service) {
            m_services[typeid(T).hash_code()] = service;
        }

        template<typename T>
        std::shared_ptr<T> Get() {
            auto it = m_services.find(typeid(T).hash_code());
            if (it != m_services.end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

    private:
        std::unordered_map<size_t, std::shared_ptr<void>> m_services;
    };
}