#pragma once
#include "Miku/Core/CoreDefinitions.h"
namespace MIKU 
{
	

}

namespace MIKU::Math 
{

	template<typename T>
	FORCEINLINE T Max(const T& a, const T& b)
	{
		return (a >= b) ? a : b;
	}

	template<typename T>
	FORCEINLINE T Min(const T& a, const T& b) 
	{
		return (a <= b) ? a : b;
	}

}