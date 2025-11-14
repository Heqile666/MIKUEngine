#include "mikupch.h"
#include "Memory.h"

namespace MIKU
{
	LinearArena* GArena = new LinearArena(nullptr, 10485760);
	
	void* ArenaRealloc(MemoryArena* arena, void* ptr, uint64 oldSize, uint64 newSize, uint64 alignment, const char* file, uint32 line) 
	{
		MIKU_CORE_ASSERT(arena, "arena is nullptr");
		void* newPtr = nullptr;
		if (newSize) 
		{
			newPtr = arena->Alloc(newSize, alignment);
			if (ptr) 
			{
				memcpy(newPtr, ptr, std::min(oldSize, newSize));
				arena->Free(ptr, oldSize);
			}
			else 
			{
				arena->Free(ptr, oldSize);
			}
		}
		return newPtr;
	}


}