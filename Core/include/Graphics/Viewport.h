#pragma once
#include <CoreExport.h>
#include <Scene/ICamera.h>

namespace chai
{
    class CORE_EXPORT IViewport
    {
    public:
        virtual ~IViewport() = default;

        virtual ICamera* getCamera() const = 0;
        virtual void setCamera(ICamera* camera) = 0;

        virtual void getViewport(int& x, int& y, int& width, int& height) const = 0;
    };
}