#pragma once
#include "Direct3D12RenderBackendCommon.h"

#include <d3dx12.h>
#include <Miku/Log.h>

namespace MIKU
{

#define D3D12_CHECK(function) {HRESULT hr = function; if(FAILED(hr)){ VerifyD3D12Result(hr, #function, __FILE__, __LINE__);} }




    static inline void VerifyD3D12Result(HRESULT result, const char* function, const char* filename, uint32 line) 
    {
        MIKU_CORE_ERROR("D3D12 function returns a runtime error. Code: 0x{0:X}. Function: {1}. File: {2}. Line: {3}.", (uint32)result, function, filename, line);
    }


    static inline D3D12_RESOURCE_FLAGS GetD3D12ResourceFlags(RenderBackendBufferCreateFlags flags)
    {
        D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
        if (EnumClassHasFlags(flags, RenderBackendBufferCreateFlags::UnorderedAccess)) 
        {
            result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }
        if (!EnumClassHasFlags(flags, RenderBackendBufferCreateFlags::ShaderResource))
        {
            result |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }
        if (EnumClassHasFlags(flags, RenderBackendBufferCreateFlags::RayTracingAccelerationStructure))
        {
            result |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;
        }
        return result;
    }


	static inline D3D12_RESOURCE_FLAGS GetD3D12ResourceFlags(RenderBackendTextureCreateFlags flags) 
	{
		D3D12_RESOURCE_FLAGS result = D3D12_RESOURCE_FLAG_NONE;
		if (EnumClassHasFlags(flags, RenderBackendTextureCreateFlags::UnorderedAccess))
		{
			result |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}
        if (!EnumClassHasFlags(flags, RenderBackendTextureCreateFlags::ShaderResource))
        {
            result |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }
        if (EnumClassHasFlags(flags, RenderBackendTextureCreateFlags::RenderTarget))
        {
            result |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
        if (EnumClassHasFlags(flags, RenderBackendTextureCreateFlags::DepthStencil))
        {
            result |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }
        return result;
	}

    static inline D3D12_HEAP_TYPE GetD3D12HeapType(RenderBackendBufferCreateFlags flags) 
    {
        D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT;
        if (EnumClassHasFlags(flags, RenderBackendBufferCreateFlags::Upload)) 
        {
            type = D3D12_HEAP_TYPE_UPLOAD;
        }
        else if (EnumClassHasFlags(flags,RenderBackendBufferCreateFlags::Readback)) 
        {
            type = D3D12_HEAP_TYPE_READBACK;
        }
        else if (EnumClassHasFlags(flags,RenderBackendBufferCreateFlags::CpuOnly)) 
        {
            type = D3D12_HEAP_TYPE_UPLOAD;
        }
    
        return type;
    }
}