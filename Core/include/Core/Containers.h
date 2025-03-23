#pragma once

#include <CoreExport.h>
#include <vector>
#include <string>

namespace chai
{
    typedef std::string CString;

    template <typename T>
    using CVector = std::vector<T>;
}
