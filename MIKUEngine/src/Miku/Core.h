#pragma once
#ifdef MIKU_PLATFORM_WINDOWS
	#ifdef MIKU_BUILD_DLL
		#define MIKU_API __declspec(dllexport)
	#else
		#define MIKU_API __declspec(dllimport)
	#endif // MIKU_BUILD_DLL
#else
#error MIKU only support Windows!
#endif // MIKU_PLATFORM_WINDOWS
