#pragma once
#include "Miku/Core/CoreTypes.h"
#include "RenderBackendTextureFormat.h"
namespace MIKU 
{
	using PhysicalDeviceID = uint32;

    enum class RenderBackendBufferCreateFlags
    {
        None = 0,
        // Update
        Upload = (1 << 0),
        Readback = (1 << 1),
        // Usage
        UnorderedAccess = (1 << 2),
        CopySrc = (1 << 3),
        CopyDst = (1 << 4),
        IndirectArguments = (1 << 5),
        ShaderResource = (1 << 6),
        VertexBuffer = (1 << 7),
        IndexBuffer = (1 << 8),
        UniformBuffer = (1 << 9),
        AccelerationStruture = (1 << 10),
        ShaderBindingTable = (1 << 11),
        // Memory access
        CreateMapped = (1 << 12),
        CpuOnly = (1 << 13),
        GpuOnly = (1 << 14),
        CpuToGpu = (1 << 15),
        GpuToCpu = (1 << 16),
    };
    MIKU_ENUM_CLASS_OPERATORS(RenderBackendBufferCreateFlags);

	struct RenderBackendSwapChainDesc 
	{
		uint32 width;
		uint32 height;
		uint64 windowHandle;
		uint32 numBuffers;
		bool vsync;
		bool fullScreen;
		RenderBackendTextureFormat format;
	
	
	};

	struct RenderBackendBufferDesc 
	{
        RenderBackendBufferDesc() = default;
        RenderBackendBufferDesc(uint32 elementSize, uint32 elementCount, RenderBackendBufferCreateFlags flags)
            : elementSize(elementSize)
            , elementCount(elementCount)
            , size(elementSize* elementCount)
            , flags(flags) {}

        FORCEINLINE bool operator==(const RenderBackendBufferDesc& rhs) const
        {
            return size == rhs.size
                && elementSize == rhs.size
                && elementCount == rhs.elementCount
                && flags == rhs.flags;
        
        }

        uint64 size;
        uint32 elementSize;
        uint32 elementCount;
        RenderBackendBufferCreateFlags flags;

        static RenderBackendBufferDesc Create(uint32 elementSize, uint32 elementCount, RenderBackendBufferCreateFlags flags) 
        {
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }
	    
        static RenderBackendBufferDesc CreateUpload(uint64 bytes) 
        {
            auto flags = RenderBackendBufferCreateFlags::Upload | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }
	    
        static RenderBackendBufferDesc CreateUpload(uint64 bytes, RenderBackendBufferCreateFlags flags)
        {
            flags |= RenderBackendBufferCreateFlags::Upload | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateReadback(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::Readback | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateIndirectArguments(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::IndirectArguments | RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateIndirectArguments(uint32 elementSize, uint32 elementCount)
        {
            auto flags = RenderBackendBufferCreateFlags::IndirectArguments | RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }

        static RenderBackendBufferDesc CreateByteAddress(uint64 bytes, bool dynamic = false)
        {
            auto flags = RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            if (dynamic)
            {
                flags |= RenderBackendBufferCreateFlags::CpuToGpu;
            }
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateStructured(uint32 elementSize, uint32 elementCount)
        {
            auto flags = RenderBackendBufferCreateFlags::UnorderedAccess | RenderBackendBufferCreateFlags::ShaderResource;
            return RenderBackendBufferDesc(elementSize, elementCount, flags);
        }

        static RenderBackendBufferDesc CreateShaderBindingTable(uint64 bytes)
        {
            auto flags = RenderBackendBufferCreateFlags::ShaderBindingTable | RenderBackendBufferCreateFlags::CpuOnly | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)(bytes >> 2), flags);
        }

        static RenderBackendBufferDesc CreateShaderBindingTable(uint64 handleSize, uint32 handleCount)
        {
            auto flags = RenderBackendBufferCreateFlags::ShaderBindingTable | RenderBackendBufferCreateFlags::CpuOnly | RenderBackendBufferCreateFlags::CreateMapped;
            return RenderBackendBufferDesc(4, (uint32)((handleSize * handleCount) >> 2), flags);
        }
	};


}