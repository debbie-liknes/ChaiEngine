#pragma once
#include <CoreExport.h>

#include <Plugin/PluginBase.h>
#include <memory>
#include <typeindex>

namespace chai
{
    static constexpr uint32_t INVALID_HANDLE = 0xFFFFFFFF;

    struct CORE_EXPORT Handle
    {
        uint32_t m_index;
        uint32_t m_generation;
        std::type_index m_type;

		Handle() : m_index(INVALID_HANDLE), m_generation(0), m_type(typeid(void)) {}
        Handle(uint32_t idx, uint32_t gen, std::type_index t)
			: m_index(idx), m_generation(gen), m_type(t) {}


        bool operator==(const Handle& other) const 
        {
            return m_index == other.m_index;
        }

        bool operator!=(const Handle& other) const 
        {
            return m_index != other.m_index;
        }

        bool isValid() const 
        {
            return m_index != INVALID_HANDLE;
		}
    };

	struct AssetHandle : public Handle
    {};

    struct ResourceHandle : public Handle
    {};
}