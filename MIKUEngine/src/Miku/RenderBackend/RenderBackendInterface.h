#pragma once
#include <Miku/Core/CoreTypes.h>
#include <Miku/RenderBackend/RenderBackendTypes.h>
#include <Miku/RenderBackend/RenderBackendHandles.h>


namespace MIKU 
{

	class RenderBackendCommandList;

	using PhysicalDeviceID = uint32;

	enum class RenderBackendType 
	{
		UnKnown,
		D3D12,
		Vulkan,
	};


	enum class RenderBackendFeature
	{
		HardwareRayTracing,
	};

	struct RenderBackendDesc
	{
		RenderBackendType              type;
		const char* applicationName;
		uint32                         applicationVersion;
		const char* engineName;
		uint32                         engineVersion;
		bool                           enableDebugLayer;
		const RenderBackendFeature* features;
		uint32                         featureCount;
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
		
		virtual RenderBackendBufferHandle CreateBuffer(uint32 deviceMask, const RenderBackendBufferDescription* desc, const void* data, const char* name) = 0;
		virtual void ResizeBuffer(RenderBackendBufferHandle buffer, uint64 size) = 0;
		virtual void MapBuffer(RenderBackendBufferHandle buffer, void** data) = 0;
		virtual void UnmapBuffer(RenderBackendBufferHandle buffer) = 0;
		virtual void UpdateBuffer(RenderBackendBufferHandle buffer, uint64 offset, const void* data, uint64 size) = 0;
		virtual void DestoryBuffer(RenderBackendBufferHandle buffer) = 0;

		virtual RenderBackendTextureHandle CreateTexture(uint32 deviceMask, const RenderBackendTextureDesc* desc, const void* data, const char* name) = 0;
		virtual void DestroyTexture(RenderBackendTextureHandle texture) = 0;
		virtual void UploadTexture(RenderBackendTextureHandle handle, const RenderBackendTextureUploadDataDesc& data) = 0;
		virtual void GetTextureReadbackData(RenderBackendTextureHandle texture, void** data) = 0;

		//virtual RenderBackendTextureSRVHandle CreateTextureSRV(uint32 deviceMask, const RenderBackendTextureSRVDesc* desc, const char* name) = 0;
		//virtual RenderBackendTextureUAVHandle CreateTextureUAV(uint32 deviceMask, const RenderBackendTextureUAVDesc* desc, const char* name) = 0;
		virtual int32 GetTextureSRVDescriptorIndex(uint32 deviceMask, RenderBackendTextureHandle srv) = 0;
		virtual int32 GetTextureUAVDescriptorIndex(uint32 deviceMask, RenderBackendTextureHandle uav) = 0;
		virtual int32 GetBufferUAVDescriptorIndex(uint32 deviceMask, RenderBackendBufferHandle uav) = 0;

		virtual RenderBackendSamplerHandle CreateSampler(uint32 deviceMask, const RenderBackendSamplerDesc* desc, const char* name) = 0;
		virtual void DestroySampler(RenderBackendSamplerHandle sampler) = 0;

		virtual RenderBackendShaderHandle CreateShader(uint32 deviceMask, const RenderBackendShaderDesc* desc, const char* name) = 0;
		virtual void DestroyShader(RenderBackendShaderHandle shader) = 0;

		virtual RenderBackendTimingQueryHeapHandle CreateTimingQueryHeap(uint32 deviceMask, const RenderBackendTimingQueryHeapDesc* desc, const char* name) = 0;
		virtual void DestroyTimingQueryHeap(RenderBackendTimingQueryHeapHandle timingQueryHeap) = 0;

		virtual void SubmitCommandList(RenderBackendCommandList** commandLists, uint32 numCommandLists, RenderBackendSwapChainHandle swapChain) = 0;

		virtual void SetObjectName(RenderBackendTextureHandle handle, const char* name) = 0;

		virtual void SetObjectName(RenderBackendBufferHandle handle, const char* name) = 0;

		void UpdateBuffer(RenderBackendBufferHandle handle, uint64 offset, const void* data, uint64 size)
		{
			void* bufferAllocation = nullptr;
			MapBuffer(handle, &bufferAllocation);
			memcpy(bufferAllocation, data, size); // TODO: offset
			UnmapBuffer(handle);
		}

	};


	RenderBackend* RenderBackendCreateInstance(const RenderBackendDesc* desc);

	void RenderBackendDestroyInstance(RenderBackend* backend);
}