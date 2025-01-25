#pragma once

#include <CoreExport.h>
#include <vector>

namespace Core
{
    class CORE_EXPORT CString {
    public:
        CString();
        CString(const char* str);
        ~CString();

    private:
        struct Impl;
        Impl* impl;
    };

    template <typename T>
    using CVector = std::vector<T>;
}
