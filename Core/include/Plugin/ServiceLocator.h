#pragma once
#include <memory>
#include <unordered_map>

namespace chai
{
    class ServiceLocator {
    public:
        template<typename T>
        static void Register(std::shared_ptr<T> service) {
            services()[typeid(T).hash_code()] = service;
        }

        template<typename T>
        static std::shared_ptr<T> Get() {
            auto it = services().find(typeid(T).hash_code());
            if (it != services().end()) {
                return std::static_pointer_cast<T>(it->second);
            }
            return nullptr;
        }

    private:
        static std::unordered_map<size_t, std::shared_ptr<void>>& services() {
            static std::unordered_map<size_t, std::shared_ptr<void>> s;
            return s;
        }
    };

}