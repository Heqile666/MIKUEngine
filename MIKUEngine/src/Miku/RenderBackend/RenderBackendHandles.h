#pragma once
#include "Miku/Core/CoreDefinitions.h"
#include <limits>
namespace MIKU 
{

	class RenderBackendHandleBase
	{
	public:
		RenderBackendHandleBase() = default;
		explicit RenderBackendHandleBase(uint64 value) :index((uint32)(value >> 32)), deviceMask((uint32)value) {}
		explicit RenderBackendHandleBase(uint32 index, uint32 deviceMask) :index(index), deviceMask(deviceMask) {}
		bool IsValid() const { return index != InvalidIndex; }
		operator bool() const { return IsValid(); }
		bool operator==(const RenderBackendHandleBase& rhs) const { return ((index == rhs.index) && (deviceMask == rhs.deviceMask)); }
		bool operator!=(const RenderBackendHandleBase& rhs) const { return ((index != rhs.index) && (deviceMask != rhs.deviceMask)); }
		uint64 ToUnit64() const { return uint64(index) << 32 | uint64(deviceMask); }
		uint32 GetIndex() const { return index; }
		uint32 GetDeviceMask() const { return deviceMask; }
	private:
		static const uint32 InvalidIndex = std::numeric_limits<uint32>::max();
		uint32 index = InvalidIndex;
		uint32 deviceMask = 0;

	};

	template<typename ObjectType>
	class RenderBackendHandle :public RenderBackendHandleBase
	{
	public:
		static const RenderBackendHandle Null;
		RenderBackendHandle() = default;
		RenderBackendHandle(uint64 value) :RenderBackendHandleBase(value) {}
		RenderBackendHandle(uint32 index, uint32 deviceMask) :RenderBackendHandleBase(index, deviceMask) {}

	};
	
	template<typename ObjectType>
	const RenderBackendHandle<ObjectType> RenderBackendHandle<ObjectType>::Null = RenderBackendHandle<ObjectType>();


	namespace RenderBackendHandleTypes
	{
		class Texture;
		class TextureView;
		class Buffer;
		class BufferView;
		class Sampler;
		class SwapChain;
		class TimingQueryHeap;
		class Shader;
		class ShaderModule;
		class RayTracingAccelerationStructure;
		class RayTracingPipelineState;
	}

	using RenderBackendTextureHandle = RenderBackendHandle<RenderBackendHandleTypes::Texture>;
	using RenderBackendTextureViewHandle = RenderBackendHandleTypes::TextureView*;
	using RenderBackendBufferHandle = RenderBackendHandle<RenderBackendHandleTypes::Buffer>;
	using RenderBackendBufferViewHandle = RenderBackendHandleTypes::BufferView*;
	using RenderBackendSamplerHandle = RenderBackendHandle<RenderBackendHandleTypes::Sampler>;
	using RenderBackendSwapChainHandle = RenderBackendHandle<RenderBackendHandleTypes::SwapChain>;
	using RenderBackendTimingQueryHeapHandle = RenderBackendHandle<RenderBackendHandleTypes::TimingQueryHeap>;
	using RenderBackendShaderHandle = RenderBackendHandle<RenderBackendHandleTypes::Shader>;
	using RenderBackendShaderModuleHandle = RenderBackendHandle<RenderBackendHandleTypes::ShaderModule>;
	using RenderBackendRayTracingAccelerationStructureHandle = RenderBackendHandle<RenderBackendHandleTypes::RayTracingAccelerationStructure>;
	using RenderBackendRayTracingPipelineStateHandle = RenderBackendHandle<RenderBackendHandleTypes::RayTracingPipelineState>;
}