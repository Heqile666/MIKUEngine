#pragma once

#ifdef MIKU_PLATFORM_WINDOWS
#if MIKU_DYNAMIC_LINK
	#ifdef MIKU_BUILD_DLL
		#define MIKU_API __declspec(dllexport)
	#else
		#define MIKU_API __declspec(dllimport)
	#endif // MIKU_BUILD_DLL
#else
	#define MIKU_API
#endif
#else
#error MIKU only support Windows!
#endif // MIKU_PLATFORM_WINDOWS

#ifdef MIKU_DEBUG
	#define MIKU_ENABLE_ASSERTS
#endif

#ifdef  MIKU_ENABLE_ASSERTS
	#define MIKU_ASSERT(x,...) {if(!(x)){MIKU_ERROR("Assertion Failed: {0}",__VA_ARGS__); __debugbreak();}}
	#define MIKU_CORE_ASSERT(x,...) {if(!(x)){MIKU_CORE_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}
#else
	#define MIKU_ASSERT(x,...)
	#define MIKU_CORE_ASSERT(x,...) 	
#endif //  MIKU_ENABLE_ASSERTS

#if defined(_MSC_VER)
#define FORCEINLINE __forceinline
#endif

#define MIKU_ENUM_CLASS_OPERATORS(EnumClass) inline constexpr EnumClass& operator|=(EnumClass& lhs, EnumClass rhs)   { return lhs = (EnumClass)((__underlying_type(EnumClass))lhs | (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr EnumClass& operator&=(EnumClass& lhs, EnumClass rhs)   { return lhs = (EnumClass)((__underlying_type(EnumClass))lhs & (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr EnumClass& operator^=(EnumClass& lhs, EnumClass rhs)   { return lhs = (EnumClass)((__underlying_type(EnumClass))lhs ^ (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr EnumClass  operator| (EnumClass  lhs, EnumClass rhs)   { return (EnumClass)((__underlying_type(EnumClass))lhs | (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr EnumClass  operator& (EnumClass  lhs, EnumClass rhs)   { return (EnumClass)((__underlying_type(EnumClass))lhs & (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr EnumClass  operator^ (EnumClass  lhs, EnumClass rhs)   { return (EnumClass)((__underlying_type(EnumClass))lhs ^ (__underlying_type(EnumClass))rhs); } \
                                           inline constexpr bool       operator! (EnumClass  e)                    { return !(__underlying_type(EnumClass))e; } \
                                           inline constexpr EnumClass  operator~ (EnumClass  e)                    { return (EnumClass)~(__underlying_type(EnumClass))e; }

#define BIT(x) (1<<x)

#define MIKU_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

