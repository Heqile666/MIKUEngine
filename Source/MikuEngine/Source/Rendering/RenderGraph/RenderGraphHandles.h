#pragma once

#include "RenderGraphCommon.h"

namespace MIKU 
{
	class RenderGraphHandleBase 
	{
	private:
		static constexpr uint32 InvalidIndex = UINT32_MAX;

		uint32 index;

		uint32 version;
	public:
		
		RenderGraphHandleBase() 
			:index(InvalidIndex)
			,version(0)
		{
			
		
		}

		explicit RenderGraphHandleBase(uint32 index, uint32 version = 0) 
		:index(index)
		,version(version)
		{
			
		}

		uint32 GetIndex() const 
		{
			return index;
		}

		uint32 GetVersion() const
		{
			return version;
		}

		bool IsNull()  const
		{
			return index == InvalidIndex;
		}

		explicit operator bool() const 
		{
			return !IsNull();
		}

		bool operator==(const RenderGraphHandleBase& rhs) const 
		{
			return ((index == rhs.index) && (version == rhs.version));
		}

		bool operator!=(const RenderGraphHandleBase& rhs) const
		{
			return ((index != rhs.index) || (version != rhs.version));
		}

		RenderGraphHandleBase& operator++() 
		{
			index++; return *this;
		}

		RenderGraphHandleBase& operator--()
		{
			index--; return *this;
		}
	};

	template <typename ObjectType>
	class RnederGraphHandle : public RenderGraphHandleBase 
	{
		static const RenderGraphHandle Null;

		static RenderGraphHandle CreateNewVersion()
		{
		
		}
	
	
	
	
	};



	

}