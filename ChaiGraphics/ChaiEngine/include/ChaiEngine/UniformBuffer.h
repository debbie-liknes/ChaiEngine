#pragma once
#include <ChaiGraphicsExport.h>
#include <memory>
#include <cstring>
#include <ChaiMath.h>

namespace chai
{
    enum class UniformType 
    {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        MAT3,
        MAT4,
        INT,
        BOOL,
        SAMPLER2D,
        UNKNOWN
    };

    class UniformBufferBase
    {
    public:
        UniformBufferBase() = default;
        virtual ~UniformBufferBase();

        virtual uint64_t getId() const = 0;
        virtual UniformType getType() const = 0;
        virtual size_t getSize() const = 0;
        virtual void getData(void* dest, size_t maxSize) const = 0;
        virtual std::unique_ptr<UniformBufferBase> clone() const = 0;

        bool isFloat() const { return getType() == UniformType::FLOAT; }
        bool isVec2() const { return getType() == UniformType::VEC2; }
        bool isVec3() const { return getType() == UniformType::VEC3; }
        bool isVec4() const { return getType() == UniformType::VEC4; }
        bool isMat4() const { return getType() == UniformType::MAT4; }
        bool isInt() const { return getType() == UniformType::INT; }
        bool isBool() const { return getType() == UniformType::BOOL; }

    protected:
        static uint64_t getNextId();
    };

    template<typename T>
    struct UniformTypeTraits 
    {
        static constexpr UniformType value = UniformType::UNKNOWN;
    };

    // Specializations for supported types
    template<> struct UniformTypeTraits<float> 
    {
        static constexpr UniformType value = UniformType::FLOAT;
    };
    template<> struct UniformTypeTraits<Vec2> 
    {
        static constexpr UniformType value = UniformType::VEC2;
    };
    template<> struct UniformTypeTraits<Vec3> 
    {
        static constexpr UniformType value = UniformType::VEC3;
    };
    template<> struct UniformTypeTraits<Vec4> 
    {
        static constexpr UniformType value = UniformType::VEC4;
    };
    template<> struct UniformTypeTraits<Mat3> 
    {
        static constexpr UniformType value = UniformType::MAT3;
    };
    template<> struct UniformTypeTraits<Mat4> 
    {
        static constexpr UniformType value = UniformType::MAT4;
    };
    template<> struct UniformTypeTraits<int> 
    {
        static constexpr UniformType value = UniformType::INT;
    };
    template<> struct UniformTypeTraits<bool> 
    {
        static constexpr UniformType value = UniformType::BOOL;
    };

    template<typename T>
    class UniformBuffer : public UniformBufferBase
    {
    public:
		UniformBuffer() : m_value{}
        {
			m_id = getNextId();
        }
        explicit UniformBuffer(const T& value) : m_value(value) 
        {
			m_id = getNextId();
        }

        uint64_t getId() const override
        {
            return m_id;
		}

        UniformType getType() const override
        {
            return UniformTypeTraits<T>::value;
        }

        size_t getSize() const override
        {
            return sizeof(T);
        }

        void getData(void* dest, size_t maxSize) const override
        {
            if (maxSize >= sizeof(T)) 
            {
                std::memcpy(dest, &m_value, sizeof(T));
            }
        }

        std::unique_ptr<UniformBufferBase> clone() const override
        {
            return std::make_unique<UniformBuffer<T>>(m_value);
        }

        // Direct access for performance
        const T& getValue() const { return m_value; }
        void setValue(const T& value) { m_value = value; }

    private:
        T m_value;
        uint64_t m_id;
    };

    inline std::shared_ptr<UniformBufferBase> createUniform(float value) 
    {
        return std::make_shared<UniformBuffer<float>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(const Vec2& value) 
    {
        return std::make_shared<UniformBuffer<Vec2>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(const Vec3& value) 
    {
        return std::make_shared<UniformBuffer<Vec3>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(const Vec4& value) 
    {
        return std::make_shared<UniformBuffer<Vec4>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(const Mat4& value) 
    {
        return std::make_shared<UniformBuffer<Mat4>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(int value) 
    {
        return std::make_shared<UniformBuffer<int>>(value);
    }

    inline std::shared_ptr<UniformBufferBase> createUniform(bool value) 
    {
        return std::make_shared<UniformBuffer<bool>>(value);
    }

    template<typename T>
    inline std::shared_ptr<UniformBuffer<T>> createUniform()
    {
        return std::make_shared<UniformBuffer<T>>();
    }
}