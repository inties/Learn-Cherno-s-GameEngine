#pragma once
#ifdef ENGINE_PLATFORM_WINDOWS
	#ifdef ENGINE_BUILDING_DLL
		#define ENGINE_API __declspec(dllexport)
	#else
		#define ENGINE_API __declspec(dllimport)
	#endif
#else
	#error Only Supports Windows Platform
#endif


//#define BIND_CLASS_FUC(fc) std::bind(fc, this, std::placeholders::_1)