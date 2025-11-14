#pragma once
#include "FundamentalTypes.h"

namespace MIKU
{

	struct Extent2D
	{
		uint32 width;
		uint32 height;
	};

	struct Extent3D
	{
		uint32 width;
		uint32 height;
		uint32 depth;
	};

	struct Offset2D
	{
		int32 x;
		int32 y;
	};

	struct Offset3D
	{
		int32 x;
		int32 y;
		int32 z;
	};

	struct Rect
	{
		// offset
		int32 x;
		int32 y;

		// extent
		uint32 width;
		uint32 height;
	};
}