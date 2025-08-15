#pragma once
#include "RenderBackendCommon.h"

namespace MIKU 
{
	enum class RenderBackendType 
	{
		UnKnown,
		DX12,
		Vulkan,
	};

	class RenderBackend 
	{
	public:
		virtual RenderBackendType GetType() const { return RenderBackendType::UnKnown; }
		virtual void Tick() = 0;
		virtual void CreateRenderDevices(PhysicalDeviceID* physicalDeviceIDs, uint32 numDevices, uint32* outDeviceMasks) = 0;
		virtual void DestroyRenderDevices() = 0;
		virtual void FlushRenderDevices() = 0;
		virtual RenderBackendSwapChainHandle CreateSwapChain(uint32 deviceMask,const RenderBackendSwapChainDesc) = 0;
		virtual void DestorySwapChain(RenderBackendSwapChainHandle swapChain) = 0;
		virtual void ResizeSwapChain(RenderBackendSwapChainHandle swapChain) = 0;
		virtual bool PresentSwapChain(RenderBackendSwapChainHandle swapChain) = 0;
		virtual RenderBackendTextureHandle GetActiveSwapChainBuffer(RenderBackendSwapChainHandle swapChain) = 0;
		virtual RenderBackendBufferHandle CreateBuffer(uint32 deviceMask, const RenderBackendBufferDesc* desc, const void* data, const char* name) = 0;
		virtual void ResizeBuffer(RenderBackendBufferHandle buffer, uint64 size) = 0;
		virtual void MapBuffer(RenderBackendBufferHandle buffer, void** data) = 0;
		virtual void UnmapBuffer(RenderBackendBufferHandle buffer) = 0;
		virtual void UpdateBuffer(RenderBackendBufferHandle buffer, uint64 offset, const void* data, uint64 size) = 0;
		virtual void DestoryBuffer(RenderBackendBufferHandle buffer) = 0;
	};

}