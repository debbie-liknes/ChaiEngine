#pragma once
#include <ChaiMathExport.h>
#include <VecBase.h>
#include <memory>

namespace chai
{
    class CHAIMATH_EXPORT Vec2 : public VectorBase<Vec2, float, 2> 
    {
        friend class VectorBase<Vec2, float, 2>;
    public:
        Vec2();
        Vec2(float x, float y);
        Vec2(float value);

        // Copy and assignment
        Vec2(const Vec2& other);
        Vec2& operator=(const Vec2& other);

        ~Vec2();

        float& operator[](size_t index);
        const float& operator[](size_t index) const;

        // Named accessors
        float getX() const;
        float getY() const;
        void setX(float x);
        void setY(float y);

        float& x();
        float& y();
        const float& x() const;
        const float& y() const;

        // TODO: add static factory methods

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}