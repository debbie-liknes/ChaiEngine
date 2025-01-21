#pragma once
//#ifdef BUILD_MYLIB
//#define CHAIGRAPHICS_API __declspec(dllexport)
//#else
//#define CHAIGRAPHICS_API __declspec(dllimport)
//#endif
//
//
//namespace chai_graphics
//{
//	//An abstract interface for graphics apis
//	class CHAIGRAPHICS_API IGraphicsApi
//	{
//	};
//}
#ifndef CHAIGRAPHICS_H
#define CHAIGRAPHICS_H

// Define export/import macros
#ifdef BUILD_CHAIGRAPHICS
#define CHAIGRAPHICS_API __declspec(dllexport) // Export when building the library
#else
#define CHAIGRAPHICS_API __declspec(dllimport) // Import when using the library
#endif

// Exported function
CHAIGRAPHICS_API void MyFunction();

#endif // MYLIBRARY_H