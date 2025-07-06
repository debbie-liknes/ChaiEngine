//#pragma once
//#include <ChaiMathExport.h>
//#include <VecBase.h>
//#include <memory>
//
//namespace chai
//{
//    class CHAIMATH_EXPORT Vec3 : public VectorBase<Vec3, float, 2>
//    {
//        friend class VectorBase<Vec3, float, 2>;
//    public:
//        Vec3();
//        Vec3(float x, float y, float z);
//        explicit Vec3(float value);
//
//        // Copy and assignment
//        Vec3(const Vec3& other);
//        Vec3& operator=(const Vec3& other);
//
//        ~Vec3();
//
//        float& operator[](size_t index);
//        const float& operator[](size_t index) const;
//
//        // Named accessors
//        float getX() const;
//        float getY() const;
//        float getZ() const;
//        void setX(float x);
//        void setY(float y);
//        void setZ(float y);
//
//        float& x();
//        float& y();
//        float& z();
//        const float& x() const;
//        const float& y() const;
//        const float& z() const;
//
//        // TODO: add static factory methods
//
//    private:
//        struct Impl;
//        std::unique_ptr<Impl> pImpl;
//    };
//}