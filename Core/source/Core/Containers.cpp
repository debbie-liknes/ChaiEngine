#include <Core/Containers.h>
#include<string>
#include <cstring>

namespace Core
{

	struct CString::Impl {
		std::string name = "";
	};

	CString::CString() : impl(new Impl()) {}
	CString::~CString() { delete impl; }
	CString::CString(const char* str) : impl(new Impl({str}))
	{
	}
}