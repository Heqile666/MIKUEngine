#pragma once

#include "Direct3D12RenderBackendCommon.h"

#include <wrl/client.h>

#include <dxgi1_6.h>
#include <dxgiformat.h>
#include <dxgidebug.h>

#include <d3dcommon.h>
#include <d3d12.h>
#include <d3d12compatibility.h>
#include <d3d12sdklayers.h>
#include <d3d12shader.h>
#include <d3d12video.h>

#include <d3dx12.h>
#include <d3dx12_core.h>
#include <d3dx12_default.h>
#include <d3dx12_barriers.h>
#include <d3dx12_render_pass.h>
#include <d3dx12_state_object.h>
#include <d3dx12_root_signature.h>
#include <d3dx12_resource_helpers.h>
#include <d3dx12_pipeline_state_stream.h>
#include <d3dx12_property_format_table.h>
#include <d3dx12_check_feature_support.h>

#include "D3D12MemoryAllocator/D3D12MemAlloc.h"
#include "Direct3D12RenderBackendUtility.h"
namespace MIKU
{
	class D3D12RenderBackend;

	enum class D3D12CommandQueueType
	{
		Direct = 0,
		Compute = 1,
		Copy = 2,
		Count = 3,
		VideoEncode = 3,
		VideoDecode = 3,

	};

	inline D3D12_COMMAND_LIST_TYPE  GetD3D12CommandListType(D3D12CommandQueueType queueType)
	{
		switch (queueType)
		{
		case D3D12CommandQueueType::Direct:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case D3D12CommandQueueType::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case D3D12CommandQueueType::Copy:
			return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			assert(false && "Unknown D3D12CommandQueueType");
			return D3D12_COMMAND_LIST_TYPE_NONE;
		}

	}
	uint32 GetDepthStencilViewIndex(RenderBackendDepthStencilAccessType accessType)
	{
		switch (accessType)
		{
		case RenderBackendDepthStencilAccessType::DepthNoAccess_StencilNoAccess:
		case RenderBackendDepthStencilAccessType::DepthNoAccess_StencilWrite:
		case RenderBackendDepthStencilAccessType::DepthWrite_StencilNoAccess:
		case RenderBackendDepthStencilAccessType::DepthWrite_StencilWrite:
			return 0u;
		case RenderBackendDepthStencilAccessType::DepthReadOnly_StencilNoAccess:
		case RenderBackendDepthStencilAccessType::DepthReadOnly_StencilWrite:
			return 1u;
		case RenderBackendDepthStencilAccessType::DepthNoAccess_StencilReadOnly:
		case RenderBackendDepthStencilAccessType::DepthWrite_StencilReadOnly:
			return 2u;
		case RenderBackendDepthStencilAccessType::DepthReadOnly_StencilReadOnly:
			return 3u;
		default:
			assert(false && "Unknown D3D12DepthStencilAccessType");
			return ~0u;
		}

	}

	struct D3D12Adapter
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;

		uint32 adapterIndex;

		DXGI_ADAPTER_DESC1 desc;

		IDXGIAdapter* GetIDXGIApdater()
		{
			return dxgiAdapter.Get();

		}
	};

	struct D3D12CommandList
	{
		D3D12CommandQueueType  queueType;

		Microsoft::WRL::ComPtr<ID3D12CommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> graphicsCommandList4;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> graphicsCommandList6;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> graphicsCommandList7;

		ID3D12CommandList* GetID3D12CommandList()
		{
			return commandList.Get();
		}

		ID3D12GraphicsCommandList* GetID3D12GraphicsCommandList()
		{
			return graphicsCommandList.Get();
		}

		ID3D12GraphicsCommandList4* GetID3D12GraphicsCommandList4()
		{
			return graphicsCommandList4.Get();
		}

		ID3D12GraphicsCommandList6* GetID3D12GraphicsCommandList6()
		{
			return graphicsCommandList6.Get();
		}

		ID3D12GraphicsCommandList7* GetID3D12GraphicsCommandList7()
		{
			return graphicsCommandList7.Get();
		}
	};

	struct D3D12CommandAllocator 
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
		D3D12CommandQueueType queueType;

		ID3D12CommandAllocator* GetID3D12CommandAllocator()
		{
			return allocator.Get();
		}
	
	};

	struct D3D12CommandQueue 
	{
		D3D12CommandQueueType queueType;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;

		uint64 lastSignaledValue;

		ID3D12CommandQueue* GetID3D12CommandQueue() 
		{
			return queue.Get();
		}

		uint64 SignalFence() 
		{
			uint64 valueToSignal = ++lastSignaledValue;
			D3D12_CHECK(queue->Signal(fence.Get(),valueToSignal));
			return valueToSignal;
		}
	};

	struct D3D12SubmissionWorkload
	{
		struct Fence 
		{
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			uint64 value;
			Fence(Microsoft::WRL::ComPtr<ID3D12Fence>&& fence, uint64)
				:fence(std::move(fence))
				, value(value)
			{

			}
		};
		D3D12CommandQueue* commandQueue;

		uint64 completionFenceValue;

		std::vector<Fence> fencesToWait;
		std::vector<Fence> fencesToSignal;

		std::vector<D3D12CommandList*> commandListsToExecute;

		std::vector<D3D12CommandAllocator*> commandAllocatorsToRelease;

	};


	struct D3D12Buffer 
	{
		std::string debugName;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation;

		RenderBackendBufferCreateFlags flags;
		RenderBackendBufferDescription desc;
		D3D12_RESOURCE_DESC1 resourceDesc;
		D3D12_BARRIER_LAYOUT initialLayout;
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress;
		uint64 size;
		void* mappedData;

		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;

		int bindlessResourceDescriptorIndexCBV;
		int bindlessResourceDescriptorIndexSRV;
		int bindlessResourceDescriptorIndexUAV;

		//about raytracing
		struct ShaderBindingTable
		{
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE rayGenerationShaderRecord;
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE missShaderTable;
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE hitGroupTable;
			D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE callableShaderTable;
		};
		ShaderBindingTable shaderBindingTable;
		ID3D12Resource* GetID3D12Resource() 
		{
			return resource.Get();
		}

	};

	struct D3D12ShaderResourceView
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
		int bindlessIndex;
	};

	struct D3D12UnorderedAccessView
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
		int bindlessIndex;
	};

	struct D3D12RenderTargetView
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
	};

	struct D3D12DepthStencilView
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
	};

	struct D3D12Texture 
	{
		std::string debugName;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<D3D12MA::Allocation> allocation;

		uint32 width;
		uint32 height;
		uint32 depth;
		uint32 arraySize; //纹理的数量
		uint32 mipLevels;
		DXGI_FORMAT format;

		RenderBackendTextureType t;
		RenderBackendResourceState initialState;

		bool isSwapChainBuffer;

		D3D12_CLEAR_VALUE clearValue;

		D3D12ShaderResourceView* shaderResourceView;
		std::vector<D3D12RenderTargetView*> renderTargetViews;
		D3D12DepthStencilView* depthStencilViews[4];
		std::vector<D3D12ShaderResourceView*> shaderResourceViews;
		std::vector<D3D12UnorderedAccessView*> unorderedAccessViews;

		UINT64 totalSize = 0;
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
		std::vector<UINT64> rowSizesInBytes;
		std::vector<UINT> numRows;

		D3D12ShaderResourceView* GetShaderResourceView(uint32 mipSlice) 
		{
			return shaderResourceViews[mipSlice];
		}

		D3D12RenderTargetView* GetRenderTargetView(uint32 mipSlice)
		{
			return renderTargetViews[mipSlice];
		}

		D3D12UnorderedAccessView* GetUnorderedAccessView(uint32 mipSlice)
		{
			return unorderedAccessViews[mipSlice];
		}

		D3D12DepthStencilView* GetDepthStencilView(uint32 index)
		{
			assert(index <= 3);
			return depthStencilViews[index];
		}

		ID3D12Resource* GetID3D12Resource()
		{
			return resource.Get();
		}

		
	};

	class D3D12BindlessDescriptorAllocator
	{

	};

	struct D3D12RenderPass
	{
		bool hasDepthStencil;
		uint32 numRenderTargets;
		DXGI_FORMAT renderTargetFormats[RenderBackendMaxRenderTargetCount];
		DXGI_FORMAT depthStencilViewFormat;
	};

	struct D3D12Sampler
	{
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
		int bindlessIndex;
	};

	struct D3D12RayTracingAccelerationStructure
	{
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags;
		Microsoft::WRL::ComPtr<ID3D12Resource> accelerationStructureBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> scratchBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> instanceBuffer;
		uint32 numInstances = 0;
		int bindlessIndex = -1;
		D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometries;
	};

	struct D3D12TimingQueryHeap
	{
		Microsoft::WRL::ComPtr<ID3D12QueryHeap> queryHeap;
		uint32 maxQueryCount;

		ID3D12QueryHeap* GetID3D12QueryHeap()
		{
			return queryHeap.Get();
		}

	};

	struct D3D12DescriptorHeap 
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;

		ID3D12DescriptorHeap* GetID3D12DescriptorHeap() 
		{
			return descriptorHeap.Get();
		}
	
	};

	struct D3D12Shader 
	{
		D3D12_SHADER_BYTECODE bytecode;
		std::wstring entryFunctionName;
		uint32 hash;
	};

	struct D3D12Program
	{

	};

	struct D3D12ComputePipelineState 
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state;

		ID3D12PipelineState* GetID3D12PipelineState() 
		{
			return state.Get();
		}
	};


	struct D3D12GraphicsPipelineStateDesc 
	{
		D3D12_BLEND_DESC blendState;
		D3D12_RASTERIZER_DESC rasterizerState;
		D3D12_DEPTH_STENCIL_DESC depthStencilState;
	};

	struct D3D12GraphicsPipelineState
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> stateObject;
		
		D3D12_PRIMITIVE_TOPOLOGY_TYPE  primitiveTopologyType;

		ID3D12PipelineState* GetID3D12PipelineState() 
		{
			return stateObject.Get();
		}
	};

	struct D3D12RayTracingPipelineStateObject
	{
		Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject;

		uint32 numMissShaders = 0;
		uint32 numHitGroups = 0;
		RenderBackendRayTracingPipelineStateDesc desc;

		ID3D12StateObject* GetID3D12StateObject()
		{
			return stateObject.Get();
		}
	};

	struct D3D12SwapChain 
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain1;
		Microsoft::WRL::ComPtr<IDXGISwapChain2> dxgiSwapChain2;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> dxgiSwapChain3;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	
		uint32 width;
		uint32 height;
		DXGI_FORMAT format;

		bool windowed;
		bool vsyncEnabled;

		uint32 numBuffers;
		RenderBackendTextureHandle buffer[RenderBackendMaxSwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Fence> frameFences[RenderBackendMaxSwapChainBufferCount];

		IDXGISwapChain1* GetIDXGISwapChain1() 
		{
			return dxgiSwapChain1.Get();
		}

		IDXGISwapChain2* GetIDXGISwapChain2()
		{
			return dxgiSwapChain2.Get();
		}

		IDXGISwapChain3* GetIDXGISwapChain3()
		{
			return dxgiSwapChain3.Get();
		}

		IDXGISwapChain4* GetIDXGISwapChain4()
		{
			return dxgiSwapChain4.Get();
		}

		ID3D12Fence* GetFrameFence(uint32 bufferIndex) 
		{
			return frameFences[bufferIndex].Get();
		}

		uint32 GetBufferCount() const 
		{
			return numBuffers;
		}

		uint32 GetCurrentBackBufferIndex() const 
		{
			return dxgiSwapChain3->GetCurrentBackBufferIndex();
		}
	};

	struct D3D12RenderBackendHandleManager 
	{
		std::vector<uint32> freeIndices;
		uint32 nextIndex;

		template<typename HandleType>
		HandleType Allocate(uint32 deviceMask = ~0U) 
		{
			uint32 index = 0;
			if (!freeIndices.empty()) 
			{
				index = freeIndices.back();
				freeIndices.pop_back();
			}
			else 
			{
				nextIndex++;
				index = nextIndex;
			}
			Handle handle = HandleType(index, deviceMask);
			return handle;
		}

		template<typename HandleType>
		void Free(HandleType handle) 
		{
			uint32 index = ((RenderBackendHandle)handle).GetIndex();
			freeIndices.push_back(index);
		}
	
	};

	class D3D12Device
	{
	public:
		D3D12Device(D3D12RenderBackend* backend)
			:backend(backend)
		{
		}


		~D3D12Device()
		{

		}

		D3D12RenderBackend* backend;

		struct DeviceMask
		{
			UINT mask;
			UINT Get() const
			{
				return mask;
			}
		};

		DeviceMask mask;
		const DeviceMask& GetMask() const { return mask; }

		std::vector<D3D12SubmissionWorkload*> workloads;

		struct D3D12DescriptorAllocator 
		{
			D3D12Device* device;
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			uint32 descriptorHandleIncrementSize;
			std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> descriptorHeaps;
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> freeList;

			void Init(D3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptorsPerBlock) 
			{
				this->device = device;
				desc.NodeMask = device->GetMask().Get();
				desc.Type = type;
				desc.NumDescriptors = numDescriptorsPerBlock;
				desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				descriptorHandleIncrementSize = device->device->GetDescriptorHandleIncrementSize(type);
			}

			void AllocateBlock() 
			{
				descriptorHeaps.emplace_back();
				D3D12_CHECK(device->GetID3D12Device()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeaps.back())));
				D3D12_CPU_DESCRIPTOR_HANDLE heapStart = descriptorHeaps.back()->GetCPUDescriptorHandleForHeapStart();
				for (UINT descriptorIndex = 0; descriptorIndex < desc.NumDescriptors; descriptorIndex++) 
				{
					D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = heapStart;
					descriptorHandle.ptr += descriptorIndex * descriptorHandleIncrementSize;
					freeList.push_back(descriptorHandle);
				}
			
			}

			D3D12_CPU_DESCRIPTOR_HANDLE Allocate() 
			{
				if (freeList.empty()) 
				{
					AllocateBlock();
				}
				assert(!freeList.empty());
				D3D12_CPU_DESCRIPTOR_HANDLE handle = freeList.back();
				freeList.pop_back();
				return handle;
			}

			void Release(D3D12_CPU_DESCRIPTOR_HANDLE index) 
			{
				freeList.push_back(index);
			}
		};

	

		Microsoft::WRL::ComPtr<ID3D12CommandSignature> drawIndirectCommandSignature;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> drawIndexedIndirectCommandSignature;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> dispatchIndirectCommandSignature;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> dispatchRaysIndirectCommandSignature;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> dispatchMeshIndirectCommandSignature;

		Microsoft::WRL::ComPtr<ID3D12Device> device;
		Microsoft::WRL::ComPtr<ID3D12Device5> device5;
		Microsoft::WRL::ComPtr<ID3D12Device10> device10;

		D3D12CommandQueue* commandQueues[(uint32)D3D12CommandQueueType::Count];

		std::vector<D3D12CommandAllocator*> commandAllocators[RenderBackendQueueFamilyCount];
		std::vector<D3D12CommandList*> commandLists[RenderBackendQueueFamilyCount];

		std::map<uint32, uint32> handleRepresentations;

		std::vector<D3D12SwapChain*> swapChains;

		std::vector<D3D12Buffer*> buffers;
		std::vector<uint32> freeBuffers;

		std::vector<D3D12Texture*> textures;
		std::vector<uint32> freeTextures;

		std::vector<D3D12Sampler*> samplers;
		std::vector<uint32> freeSamplers;

		std::vector<D3D12RayTracingAccelerationStructure*> accelerationStructures;
		std::vector<uint32> freeAccelerationStructures;

		std::vector<D3D12Shader*> shaders;
		std::vector<uint32> freeShaders;


		std::unordered_map<uint64, D3D12ComputePipelineState*> computePipelineStateMap;
		std::unordered_map<uint64, D3D12GraphicsPipelineState*> graphicsPipelineStateMap;
		std::vector<D3D12RayTracingPipelineStateObject*> rayTracingPipelineStateObjects;

		Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator;

		std::vector<D3D12TimingQueryHeap*> queryHeaps;//用来测量 GPU 上某段操作花了多少时间。

		struct CopyWorkload
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList7;
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			D3D12Buffer* uploadBuffer;
		};
		std::vector<CopyWorkload> copyWorkLoadFreeList;

		D3D12DescriptorAllocator resourceDescriptorAllocator;
		D3D12DescriptorAllocator samplerDescriptorAllocator;
		D3D12DescriptorAllocator rtvDescriptorAllocator;
		D3D12DescriptorAllocator dsvDescriptorAllocator;

		//Bindless
		D3D12DescriptorHeap* resourceDescriptorHeap;
		D3D12DescriptorHeap* samplerDescriptorHeap;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

		std::vector<int> freeResourceDescriptorIndices;
		int AllocateResourceDescriptorIndex() 
		{
			if (!freeResourceDescriptorIndices.empty()) 
			{
				int index = freeResourceDescriptorIndices.back();
				freeResourceDescriptorIndices.pop_back();
				return index;
			}
			return -1;
		}

		std::vector<int> freeSamplerDescriptorIndices;
		int AllocateSamplerDescriptorIndex() 
		{
			if (!freeSamplerDescriptorIndices.empty()) 
			{
				int index = freeSamplerDescriptorIndices.back();
				freeSamplerDescriptorIndices.pop_back();
				return index;
			}
			return -1;
		}

		ID3D12RootSignature* GetID3D12RootSignature() 
		{
			return rootSignature.Get();
		}

		inline uint32 GetRenderBackendHandleRepresentation(uint32 handle)
		{
			if (handleRepresentations.find(handle) == handleRepresentations.end())
			{
				assert(false);
			}
			return handleRepresentations[handle];
		}

		inline bool TryGetRenderBackendHandleRepresentation(uint32 handle,uint32* outValue)
		{
			if (handleRepresentations.find(handle) == handleRepresentations.end()) 
			{
				return false;
			}
			*outValue = handleRepresentations[handle];
			return true;
		
		}
		inline void SetRenderBackendHandleRepresentation(uint32 handle,uint32 value) 
		{
			handleRepresentations[handle] = value;
		}

		inline bool RemoveRenderBackendHandleRepresentation(uint32 handle) 
		{
			return handleRepresentations.erase(handle);
		}

		ID3D12Device* GetID3D12Device()
		{
			return device.Get();
		}

		ID3D12Device5* GetDXRDevice()
		{
			return device5.Get();
		}

		D3D12CommandQueue* GetCOmmandQueue(D3D12CommandQueueType type) 
		{
			return commandQueues[(uint32)type];
		}

		ID3D12CommandSignature* GetDispatchIndirectCommandSignature()
		{
			return dispatchIndirectCommandSignature.Get();
		}

		ID3D12CommandSignature* GetDrawIndirectCommandSignature()
		{
			return drawIndirectCommandSignature.Get();
		}

		ID3D12CommandSignature* GetDrawIndexedIndirectCommandSignature()
		{
			return drawIndexedIndirectCommandSignature.Get();
		}

		ID3D12CommandSignature* GetDispatchMeshIndirectCommandSignature()
		{
			return dispatchMeshIndirectCommandSignature.Get();
		}

		D3D12Texture* GetTexture(RenderBackendTextureHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return textures[index];
		}

		D3D12Buffer* GetBuffer(RenderBackendBufferHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return buffers[index];
		}

		D3D12RayTracingAccelerationStructure* GetRayTracingAccelerationStructure(RenderBackendRayTracingAccelerationStructureHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return accelerationStructures[index];
		}

		D3D12Shader* GetShader(RenderBackendShaderHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return shaders[index];
		}

		D3D12TimingQueryHeap* GetTimingQueryHeap(RenderBackendTimingQueryHeapHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return queryHeaps[index];
		}

		D3D12SwapChain* GetSwapChain(RenderBackendSwapChainHandle handle)
		{
			uint32 index = GetRenderBackendHandleRepresentation(handle.GetIndex());
			return swapChains[index];
		}

		D3D12CommandQueue* GetCommandQueue(D3D12CommandQueueType type) 
		{
			return commandQueues[(uint32)type];
		}

		uint32 CreateD3D12SwapChain(const RenderBackendSwapChainDesc* desc);


		D3D12CommandAllocator* AllocateCommandAllocator(D3D12CommandQueueType queueType) 
		{
			D3D12CommandAllocator* commandAllocator = nullptr;
			auto& commandAllocatorPool = commandAllocators[(uint32)queueType];

			if (commandAllocatorPool.empty()) 
			{
				commandAllocator = new D3D12CommandAllocator();
				D3D12_CHECK(device->CreateCommandAllocator(GetD3D12CommandListType(queueType),IID_PPV_ARGS(&commandAllocator->allocator)));
				commandAllocator->queueType = queueType;
			}
			else 
			{
				commandAllocator = commandAllocatorPool.back();
				commandAllocatorPool.pop_back();
			}
			
			assert(commandAllocator);
			return commandAllocator;
		
		}

		void ReleaseCommandAllocator(D3D12CommandAllocator* allocator) 
		{
			assert(allocator);
			D3D12_CHECK(allocator->GetID3D12CommandAllocator()->Reset());
			auto& commandAllocatorPool = commandAllocators[(uint32)allocator->queueType];
			commandAllocatorPool.push_back(allocator);
		}

		D3D12CommandList* AllocatorCommandList(D3D12CommandAllocator* commandAllocator) 
		{
			D3D12CommandQueueType queueType = commandAllocator->queueType;
			D3D12CommandList* commandList = nullptr;
			auto& commandListPool = commandLists[(uint32)queueType];
			if (commandListPool.empty()) 
			{
				commandList = new D3D12CommandList();
				switch (queueType) 
				{
					case D3D12CommandQueueType::Direct:
					case D3D12CommandQueueType::Compute:
					{
						D3D12_CHECK(device->CreateCommandList(
							0,
							GetD3D12CommandListType(queueType),
							commandAllocator->GetID3D12CommandAllocator(),
							nullptr,
							IID_PPV_ARGS(&commandList->graphicsCommandList)));

						commandList->commandList = commandList->graphicsCommandList;

						D3D12_CHECK(commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->graphicsCommandList4)));
						D3D12_CHECK(commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->graphicsCommandList6)));
						D3D12_CHECK(commandList->commandList->QueryInterface(IID_PPV_ARGS(&commandList->graphicsCommandList7)));
					}break;
					case D3D12CommandQueueType::Copy:
					{
						D3D12_CHECK(device->CreateCommandList(
							0,
							GetD3D12CommandListType(queueType),
							commandAllocator->GetID3D12CommandAllocator(),
							nullptr,
							IID_PPV_ARGS(&commandList->commandList)
						));
					}break;
					default: 
					{
						assert(false);
						std::abort();
						return commandList;
					}
				}
				commandList->queueType = queueType;
				D3D12_CHECK(commandList->commandList->SetName(L"D3D12CommandList"));
			}
			else 
			{
				commandList = commandListPool.back();
				commandListPool.pop_back();
				commandList->GetID3D12GraphicsCommandList()->Reset(commandAllocator->GetID3D12CommandAllocator(), nullptr);
			}
			assert(commandList);
			return commandList;
		}

		void ReleaseCommandList(D3D12CommandList* commandList) 
		{
			assert(commandList);
			auto& commandListPool = commandLists[(uint32)commandList->queueType];
			commandListPool.push_back(commandList);
		}



		uint32 AllocateBuffer()
		{
			uint32 bufferIndex = 0;
			if (!freeBuffers.empty())
			{
				bufferIndex = freeBuffers.back();
				freeBuffers.pop_back();
			}
			else
			{
				bufferIndex = (uint32)(buffers.size());
				buffers.emplace_back(new D3D12Buffer());
			}
			return bufferIndex;
		}

		uint32 CreateD3D12QueryHeap(const RenderBackendTimingQueryHeapDesc* desc, const char* name)
		{
			uint32 index = (uint32)queryHeaps.size();
			queryHeaps.push_back(new D3D12TimingQueryHeap());

			D3D12TimingQueryHeap* queryHeap = queryHeaps[index];
			
			uint32 maxQueryCount = 2 * desc->maxRegions;
			D3D12_QUERY_HEAP_DESC queryHeapDesc =
			{
				D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
				maxQueryCount,
				mask.Get()
			};

			D3D12_CHECK(device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&queryHeap->queryHeap)));
			D3D12_CHECK(samplerDescriptorHeap->descriptorHeap->SetName(UTF8ToUTF16(name).c_str()));
			
			queryHeap->maxQueryCount = maxQueryCount;
			return index;
		}


		uint32 CreateD3D12Buffer(const RenderBackendBufferDescription* desc, const void* data, const char* name)
		{
			// 从缓冲区池中分配一个空闲缓冲区槽，返回索引
			uint32 index = AllocateBuffer();

			// 取得对应索引处的 D3D12Buffer 指针（假设 buffers 是一个 D3D12Buffer* 数组或类似结构）
			D3D12Buffer* buffer = buffers[index];

			// 构造资源描述符，描述将要创建的缓冲区资源
			D3D12_RESOURCE_DESC1 resourceDesc =
			{
				D3D12_RESOURCE_DIMENSION_BUFFER,  // 资源类型：缓冲区
				0,                                // 内存对齐大小（0表示默认）
				desc->size,                      // 资源宽度（缓冲区大小，字节数）
				1,                                // 高度，缓冲区固定为1
				1,                                // 深度或数组大小，缓冲区为1
				1,                                // Mip层数，缓冲区为1
				DXGI_FORMAT_UNKNOWN,              // 格式，缓冲区不指定格式
				{1,0},                           // 多重采样信息：采样数=1，采样质量=0（缓冲区无多采样）
				D3D12_TEXTURE_LAYOUT_ROW_MAJOR,  // 纹理内存布局，缓冲区使用行主序（线性）
				GetD3D12ResourceFlags(desc->flags) // 资源标志
			};

			// 构造D3D12MA所需的分配描述，告诉分配器如何分配显存（堆类型等）
			D3D12MA::ALLOCATION_DESC allocationDesc =
			{
				D3D12MA::ALLOCATION_FLAG_NONE,       // 默认无特殊分配标志
				GetD3D12HeapType(desc->flags),       // 根据buffer标志得到应使用的堆类型（比如默认堆、上传堆等）
				D3D12_HEAP_FLAG_NONE,                 // 堆的相关标志，一般为0
				nullptr,                             // 自定义ID（不用则置空）
				nullptr                              // 特殊自定义参数（不用则置空）
			};

			// 资源的初始布局，暂时设为未定义（GPU需要后续进入正确状态）
			D3D12_BARRIER_LAYOUT initialLayout = D3D12_BARRIER_LAYOUT_UNDEFINED;

			// 确保缓存槽内资源和分配为空，避免覆盖泄漏
			assert(buffer->resource == nullptr);
			assert(buffer->allocation == nullptr);

			// 需要注意：这里的 allocator 需要在外部初始化和赋值，否则是空指针
			Microsoft::WRL::ComPtr<D3D12MA::Allocator> allocator;

			// 通过 D3D12MA 分配器创建资源和分配显存，合并一步完成
			D3D12_CHECK(allocator->CreateResource3(
				&allocationDesc,       // 内存分配描述
				&resourceDesc,         // 资源描述
				initialLayout,         // 初始状态
				nullptr,               // 清除值，缓冲区用不到
				0,                     // 创建Flags
				nullptr,               // 保护会话，一般为 nullptr
				&buffer->allocation,   // 输出：内存分配对象
				IID_PPV_ARGS(&buffer->resource) // 输出：资源指针(ID3D12Resource)
			));

			// 给资源命名，方便调试和工具追踪
			D3D12_CHECK(buffer->resource->SetName(UTF8ToUTF16(name).c_str()));

			// 设置缓冲区对象其他信息
			buffer->debugName = name;                // 保存名称
			buffer->desc = *desc;                    // 缓冲区描述信息的复制
			buffer->resourceDesc = resourceDesc;    // 资源描述保存到buffer，便于未来查询
			buffer->initialLayout = initialLayout;  // 初始状态，也保存下来
			buffer->gpuAddress = buffer->resource->GetGPUVirtualAddress();  // GPU虚拟地址，用于直接访问gpu资源
			buffer->flags = desc->flags;             // 标记保存
			buffer->size = desc->size;               // 大小保存
			// 初始时绑定不可用，先置-1
			buffer->bindlessResourceDescriptorIndexCBV = -1;
			buffer->bindlessResourceDescriptorIndexSRV = -1;
			buffer->bindlessResourceDescriptorIndexUAV = -1;

			// 如果缓冲区标志中启用了“Readback”，意味着CPU需要读该缓冲区
			// 需要调用 Map，把资源映射到CPU地址空间，这样CPU才能访问缓冲区数据
			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::Readback))
			{
				D3D12_CHECK(buffer->resource->Map(0, nullptr, &buffer->mappedData));//映射,表示CPU只读
				// mappedData 就是CPU侧对应的映射指针，可以操作其中数据
			}
			else if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::Upload))
			{
				D3D12_RANGE readRange = { 0,0 };
				//&buffer->mappedData,返回映射后的cpu地址
				D3D12_CHECK(buffer->resource->Map(0, &readRange, &buffer->mappedData));//映射操作，CPU只写数据到上传堆不读
			}
			else
			{
				// The mapped data pointer is valid only if the resource is created with CPU access (upload or readback).
				buffer->mappedData = nullptr;
			}

			if (data != nullptr)
			{
				if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::Upload)) // 如果是上传堆资源，直接copy到上传堆
				{
					assert(buffer->mappedData);
					memcpy(buffer->mappedData, data, buffer->size);//这里才是写数据到GPU
				}
				else // Copy throught upload heap
				{
					//将数据写入上传堆在写到默认堆上去
					CopyWorkload copyWorkload = AllocateCopyWorkload(buffer->size);
					memcpy(copyWorkload.uploadBuffer->mappedData, data, buffer->size);
					copyWorkload.commandList->Reset(copyWorkload.commandAllocator.Get(), nullptr);

					{
						D3D12_BUFFER_BARRIER BufBarriers[] =
						{
							CD3DX12_BUFFER_BARRIER(
								D3D12_BARRIER_SYNC_ALL,
								D3D12_BARRIER_SYNC_ALL,
								D3D12_BARRIER_ACCESS_NO_ACCESS,
								D3D12_BARRIER_ACCESS_COPY_DEST,
								buffer->GetID3D12Resource()
							)
						};

						D3D12_BARRIER_GROUP BufBarrierGroups[] =
						{
							CD3DX12_BARRIER_GROUP(1,BufBarriers)
						};
						
						copyWorkload.commandList7->Barrier(1, BufBarrierGroups);
					}

					copyWorkload.commandList->CopyBufferRegion(
						buffer->GetID3D12Resource(),
						0,
						copyWorkload.uploadBuffer->GetID3D12Resource(),
						0,
						buffer->size);

					SubmitCopyWorkload(copyWorkload);
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::UniformBuffer)) 
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbcDesc = 
				{
					buffer->gpuAddress,
					UINT(buffer->size)
				};
			
				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				device->CreateConstantBufferView(&cbcDesc, buffer->descriptor);

				buffer->bindlessResourceDescriptorIndexCBV = AllocateResourceDescriptorIndex();
				if (buffer->bindlessResourceDescriptorIndexCBV >= 0) 
				{
					assert(buffer->bindlessResourceDescriptorIndexCBV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexCBV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CopyDescriptorsSimple(1, rangeStart, buffer->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::ShaderResource)) 
			{
				bool isStructuredBuffer = EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::StructuredBuffer);

				D3D12_BUFFER_SRV bufferSRV =
				{
					0,
					isStructuredBuffer ? desc->elementCount : uint32(buffer->size / sizeof(uint32)),
					isStructuredBuffer ? desc->elementSize : 0,
					isStructuredBuffer ? D3D12_BUFFER_SRV_FLAG_NONE : D3D12_BUFFER_SRV_FLAG_RAW
				};

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc =
				{
					isStructuredBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS,
					D3D12_SRV_DIMENSION_BUFFER,
					D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
					bufferSRV
				};

				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				device->CreateShaderResourceView(buffer->GetID3D12Resource(), &srvDesc, buffer->descriptor);

				buffer->bindlessResourceDescriptorIndexSRV = AllocateResourceDescriptorIndex();
				if (buffer->bindlessResourceDescriptorIndexSRV >= 0) 
				{
					assert(buffer->bindlessResourceDescriptorIndexSRV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexSRV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CopyDescriptorsSimple(1, rangeStart, buffer->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);;
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::UnorderedAccess)) 
			{
				bool isStructuredBuffer = EnumClassHasFlags(desc->flags,RenderBackendBufferCreateFlags::StructuredBuffer);

				D3D12_BUFFER_UAV bufferUAV =
				{
					0,
					isStructuredBuffer ? desc->elementCount : uint32(buffer->size / sizeof(uint32)),
					isStructuredBuffer ? desc->elementSize : 0,
					0,
					isStructuredBuffer ? D3D12_BUFFER_UAV_FLAG_NONE : D3D12_BUFFER_UAV_FLAG_RAW
				};

				//创建描述符所需要的结构体
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc =
				{
					isStructuredBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS,
					D3D12_UAV_DIMENSION_BUFFER,
					bufferUAV
				};
			
				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				device->CreateUnorderedAccessView(buffer->GetID3D12Resource(),nullptr,&uavDesc,buffer->descriptor);

				buffer->bindlessResourceDescriptorIndexUAV = AllocateResourceDescriptorIndex();
				if(buffer->bindlessResourceDescriptorIndexUAV >= 0)
				{
					assert(buffer->bindlessResourceDescriptorIndexUAV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexUAV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CopyDescriptorsSimple(1,rangeStart,buffer->descriptor,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}

			return index;
		}

		void ResizeD3D12Buffer(uint32 index, uint64 size) 
		{
			D3D12Buffer* buffer = buffers[index];
			D3D12MA::ALLOCATION_DESC allocationDesc = {
				D3D12MA::ALLOCATION_FLAG_NONE,
				GetD3D12HeapType(buffer->flags),
				D3D12_HEAP_FLAG_NONE,
				nullptr,
				nullptr
			};

			buffer->resourceDesc.Width = size;


			//通过D3D12MA创建GPU资源
			D3D12_CHECK(allocator->CreateResource3(
				&allocationDesc,
				&buffer->resourceDesc,
				buffer->initialLayout,
				nullptr,
				0,
				nullptr,
				&buffer->allocation,
				IID_PPV_ARGS(&buffer->resource)));

			D3D12_CHECK(buffer->resource->SetName(UTF8ToUTF16(buffer->debugName).c_str()));
			buffer->gpuAddress = buffer->resource->GetGPUVirtualAddress();
			buffer->size = size;
			buffer->bindlessResourceDescriptorIndexCBV = -1;
			buffer->bindlessResourceDescriptorIndexSRV = -1;
			buffer->bindlessResourceDescriptorIndexUAV = -1;

			if (EnumClassHasFlags(buffer->flags, RenderBackendBufferCreateFlags::Readback)) 
			{
				D3D12_CHECK(buffer->resource->Map(0,nullptr,&buffer->mappedData));
			}
			else if (EnumClassHasFlags(buffer->flags, RenderBackendBufferCreateFlags::Upload)) 
			{
				D3D12_RANGE readRange = {
					0,
					0
				};
				D3D12_CHECK(buffer->resource->Map(0,&readRange,&buffer->mappedData));
			}
			else 
			{
				buffer->mappedData = nullptr;
			}
			

			//TODO:
			RenderBackendBufferDescription* desc = &buffer->desc;
			desc->elementCount = uint32(size / desc->elementSize);


			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::UniformBuffer))
			{

				//创建描述符结构体
				D3D12_CONSTANT_BUFFER_VIEW_DESC cbcDesc =
				{
					buffer->gpuAddress,
					UINT(buffer->size)
				};

				//从描述符对中获取句柄
				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				//创建描述符
				device->CreateConstantBufferView(&cbcDesc,buffer->descriptor);


				//将创建的资源复制到bindless中
				buffer->bindlessResourceDescriptorIndexCBV = AllocateResourceDescriptorIndex();
				if (buffer->bindlessResourceDescriptorIndexCBV >= 0)
				{
					assert(buffer->bindlessResourceDescriptorIndexCBV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexCBV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CopyDescriptorsSimple(1,rangeStart,buffer->descriptor,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::ShaderResource)) 
			{
				bool isStructureBuffer = EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::StructuredBuffer);
				D3D12_BUFFER_SRV bufferSRV =
				{
					0,
					isStructureBuffer ? desc->elementCount:uint32(buffer->size/sizeof(uint32)),
					isStructureBuffer ? desc->elementSize:0,
					isStructureBuffer ? D3D12_BUFFER_SRV_FLAG_NONE:D3D12_BUFFER_SRV_FLAG_RAW
				};

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc =
				{
					isStructureBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS,
					D3D12_SRV_DIMENSION_BUFFER,
					D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
					bufferSRV
				};

				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				device->CreateShaderResourceView(buffer->GetID3D12Resource(), &srvDesc, buffer->descriptor);

				buffer->bindlessResourceDescriptorIndexSRV = AllocateResourceDescriptorIndex();
				if (buffer->bindlessResourceDescriptorIndexSRV >= 0) 
				{
					assert(buffer->bindlessResourceDescriptorIndexSRV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexSRV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					device->CopyDescriptorsSimple(1,rangeStart,buffer->descriptor,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
			if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::UnorderedAccess)) 
			{
				bool isStructureBuffer = EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::StructuredBuffer);
				D3D12_BUFFER_UAV bufferUAV =
				{
					0,
					isStructureBuffer ? desc->elementCount : uint32(buffer->size / sizeof(uint32)),
					isStructureBuffer ? desc->elementSize : 0,
					0,
					isStructureBuffer ? D3D12_BUFFER_UAV_FLAG_NONE : D3D12_BUFFER_UAV_FLAG_RAW
				};

				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc =
				{
					isStructureBuffer ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R32_TYPELESS,
					D3D12_UAV_DIMENSION_BUFFER,
					bufferUAV
				};

				buffer->descriptor = resourceDescriptorAllocator.Allocate();
				device->CreateUnorderedAccessView(buffer->GetID3D12Resource(),nullptr,&uavDesc,buffer->descriptor);

				buffer->bindlessResourceDescriptorIndexUAV = AllocateResourceDescriptorIndex();
				if (buffer->bindlessResourceDescriptorIndexUAV >= 0) 
				{
					assert(buffer->bindlessResourceDescriptorIndexUAV < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
					D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
					rangeStart.ptr += buffer->bindlessResourceDescriptorIndexUAV * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

					device->CopyDescriptorsSimple(1, rangeStart, buffer->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
			}
		}

		uint32 AllocateTexture() 
		{
			uint32 textureIndex = 0;
			if(!freeTextures.empty())
			{
				textureIndex = freeTextures.back();
				freeTextures.pop_back();
			}
			else 
			{
				textureIndex = (uint32)textures.size();
				textures.emplace_back(new D3D12Texture());
			}
			return textureIndex;
			
		}

		uint32 CreateD3D12Texture(const RenderBackendTextureDesc* desc, const void* data,const char* name) 
		{
			uint32 index = AllocateTexture();
			D3D12Texture* texture = textures[index];

			D3D12_RESOURCE_DESC1 resourceDesc =
			{
				GetD3D12ResourceDimension(desc->type),
				0,
				desc->width,
				desc->height,
				(UINT16)((desc->type == RenderBackendTextureType::Texture3D) ? desc->depth : desc->arrayLayerCount),
				(UINT16)desc->mipLevelCount,
				ConvertToDXGIFormat(desc->format),
				{1,0},
				D3D12_TEXTURE_LAYOUT_UNKNOWN,
				GetD3D12ResourceFlags(desc->flags)

			};

			//默认堆
			D3D12MA::ALLOCATION_DESC allocationDesc =
			{
				D3D12MA::ALLOCATION_FLAG_NONE,
				D3D12_HEAP_TYPE_DEFAULT,
				D3D12_HEAP_FLAG_NONE,
				nullptr,
				nullptr
			};

			// D3D12的资源屏障Barrier机制，描述该资源的用途
			D3D12_BARRIER_LAYOUT initialLayout = ConvertToD3D12BarrierLayout(desc->initialState);
			D3D12_CLEAR_VALUE optimizedClearValue = {};
			optimizedClearValue.Color[0] = desc->clearValue.colorValue.float32[0];
			optimizedClearValue.Color[1] = desc->clearValue.colorValue.float32[1];
			optimizedClearValue.Color[2] = desc->clearValue.colorValue.float32[2];
			optimizedClearValue.Color[3] = desc->clearValue.colorValue.float32[3];
			optimizedClearValue.DepthStencil.Depth = desc->clearValue.depthStencilValue.depth;
			optimizedClearValue.DepthStencil.Stencil = desc->clearValue.depthStencilValue.stencil; // TODO
			optimizedClearValue.Format = resourceDesc.Format;
			bool useClearValue = EnumClassHasFlags(desc->flags,RenderBackendTextureCreateFlags::RenderTarget)|| EnumClassHasFlags(desc->flags,RenderBackendTextureCreateFlags::DepthStencil);

			D3D12_CHECK(allocator->CreateResource3(
				&allocationDesc,
				&resourceDesc,
				initialLayout,
				useClearValue ? &optimizedClearValue : nullptr,
				0,
				nullptr,
				&texture->allocation,
				IID_PPV_ARGS(&texture->resource)
				));
			D3D12_CHECK(texture->resource->SetName(UTF8ToUTF16(name).c_str()));

			texture->width = desc->width;
			texture->height = desc->height;
			texture->depth = desc->depth;
			texture->mipLevels = desc->mipLevelCount;
			texture->arraySize = desc->arrayLayerCount;
			texture->format = resourceDesc.Format;
			texture->initialState = desc->initialState;
			texture->isSwapChainBuffer = false;
			texture->clearValue = optimizedClearValue;

			texture->t = desc->type;

			texture->debugName = name;


			//2D纹理的资源总数 = 纹理个数*单个纹理的mipmap数
			uint32 numSubresource = desc->arrayLayerCount * std::max(1u, desc->mipLevelCount);
			texture->totalSize = 0;
			texture->footprints.resize(numSubresource);
			texture->rowSizesInBytes.resize(numSubresource);
			texture->numRows.reserve(numSubresource);

			//获取纹理的GPU布局信息到texture中的各种vector中
			device10->GetCopyableFootprints1(
				&resourceDesc,
				0,
				numSubresource,
				0,
				texture->footprints.data(),
				texture->numRows.data(),
				texture->rowSizesInBytes.data(),
				&texture->totalSize
			);

			if(data != nullptr)
			{
#if 0
				std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(texture->footprints.size());
				for (uint32 i = 0; i < numSubresources; i++)
				{
					subresourceData[i] = {};
					subresourceData[i].pData = data;
					subresourceData[i].RowPitch = texture->width * RenderBackendGetTextureFormatDesc(desc->format).bytes;
					if (desc->type == RenderBackendTextureType::Texture3D)
					{
						//subresourceData[i].SlicePitch = texture->height * subresourceData[i].RowPitch;
					}
				}

				CopyWorkload copyWorkload = AllocateCopyWorkload(texture->totalSize);
				copyWorkload.commandList->Reset(copyWorkload.commandAllocator.Get(), nullptr);

				void* mappedData = copyWorkload.uploadBuffer->mappedData;
				for (uint32 subresourceIndex = 0; subresourceIndex < numSubresources; subresourceIndex++)
				{
					D3D12_MEMCPY_DEST memcpyDest = {
						.pData = (void*)((UINT64)mappedData + texture->footprints[subresourceIndex].Offset),
						.RowPitch = (SIZE_T)texture->footprints[subresourceIndex].Footprint.RowPitch,
						.SlicePitch = (SIZE_T)texture->footprints[subresourceIndex].Footprint.RowPitch * (SIZE_T)texture->numRows[subresourceIndex],
					};
					MemcpySubresource(&memcpyDest, &subresourceData[subresourceIndex], (SIZE_T)texture->rowSizesInBytes[subresourceIndex], texture->numRows[subresourceIndex], texture->footprints[subresourceIndex].Footprint.Depth);

					CD3DX12_TEXTURE_COPY_LOCATION dstTextureCopyLocation(texture->GetID3D12Resource(), subresourceIndex);
					CD3DX12_TEXTURE_COPY_LOCATION srcTextureCopyLocation(copyWorkload.uploadBuffer->GetID3D12Resource(), texture->footprints[subresourceIndex]);
					copyWorkload.commandList->CopyTextureRegion(
						&dstTextureCopyLocation,
						0,
						0,
						0,
						&srcTextureCopyLocation,
						nullptr);
				}
				SubmitCopyWorkload(copyWorkload);
#else
				//上传数据在CPU上的布局信息
				std::vector<D3D12_SUBRESOURCE_DATA> subresourceData(1);
				for (uint32 i = 0; i < 1; i++) 
				{
					subresourceData[i] = {};
					subresourceData[i].pData = data;
					subresourceData[i].RowPitch = texture->width * RenderBackendGetTextureFormatDesc(desc->format).bytes;
					if (desc->type == RenderBackendTextureType::Texture3D)
					{
						//subresourceData[i].SlicePitch = texture->height * subresourceData[i].RowPitch;
					}

				}

				//获取一个CopyWorkload,用于上传数据，CopyWorkload里面提供的是上传堆资源
				CopyWorkload copyWorkload = AllocateCopyWorkload(std::max(texture->totalSize, UINT64(4)));
				copyWorkload.commandList->Reset(copyWorkload.commandAllocator.Get(), nullptr);
				

				//在上传数据前，需要对资源进行转换
				{
					D3D12_TEXTURE_BARRIER TexBarriers[] =
					{
						//D3D12 辅助库（d3dx12.h）中的一个辅助类，用于简化 Enhanced Barriers（增强屏障，D3D12.1 + ）中子资源范围的指定
						CD3DX12_TEXTURE_BARRIER(
							D3D12_BARRIER_SYNC_ALL,
							D3D12_BARRIER_SYNC_ALL,
							D3D12_BARRIER_ACCESS_NO_ACCESS,
							D3D12_BARRIER_ACCESS_COPY_DEST,
							D3D12_BARRIER_LAYOUT_UNDEFINED,
							D3D12_BARRIER_LAYOUT_COPY_DEST,
							texture->GetID3D12Resource(),
							CD3DX12_BARRIER_SUBRESOURCE_RANGE(0xffffffff),//D3D12 辅助库（d3dx12.h）中的一个辅助类，用于简化 Enhanced Barriers（增强屏障，D3D12.1 + ）中子资源范围的指定
							D3D12_TEXTURE_BARRIER_FLAG_DISCARD //放弃纹理中的全部数据
						)

					};

					D3D12_BARRIER_GROUP TexBarrierGroups[] =
					{
						CD3DX12_BARRIER_GROUP(1,TexBarriers)
					};
				
					//todo Need a buffer barrier ?
				
					copyWorkload.commandList7->Barrier(1, TexBarrierGroups);

				}

				void* mappedData = copyWorkload.uploadBuffer->mappedData;
				for (uint32 subresourceIndex = 0; subresourceIndex < 1; subresourceIndex++)
				{
					D3D12_MEMCPY_DEST memcpyDest =
					{
						(void*)((UINT64)mappedData + texture->footprints[subresourceIndex].Offset),
						(SIZE_T)texture->footprints[subresourceIndex].Footprint.RowPitch,
						(SIZE_T)texture->footprints[subresourceIndex].Footprint.RowPitch * (SIZE_T)texture->numRows[subresourceIndex]
					};
					
					//复制纹理数据到上传堆
					MemcpySubresource(&memcpyDest,&subresourceData[subresourceIndex],(SIZE_T)texture->rowSizesInBytes[subresourceIndex],texture->numRows[subresourceIndex],texture->footprints[subresourceIndex].Footprint.Depth);

					CD3DX12_TEXTURE_COPY_LOCATION dstTextureCopyLocation(texture->GetID3D12Resource(), subresourceIndex);
					CD3DX12_TEXTURE_COPY_LOCATION srcTextureCopyLocation(copyWorkload.uploadBuffer->GetID3D12Resource(), texture->footprints[subresourceIndex]);
					
					//复制纹理数据到默认堆
					copyWorkload.commandList->CopyTextureRegion(
						&dstTextureCopyLocation,
						0,
						0,
						0,
						&srcTextureCopyLocation,
						nullptr
					);

				}
				
				{
					CD3DX12_TEXTURE_BARRIER TexBarries[] =
					{
						CD3DX12_TEXTURE_BARRIER(
							D3D12_BARRIER_SYNC_ALL,
							D3D12_BARRIER_SYNC_ALL,
							D3D12_BARRIER_ACCESS_COPY_DEST,
							D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
							D3D12_BARRIER_LAYOUT_COPY_DEST,
							D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
							texture->GetID3D12Resource(),
							CD3DX12_BARRIER_SUBRESOURCE_RANGE(0xffffffff),//所有子资源
							D3D12_TEXTURE_BARRIER_FLAG_NONE
						)
					};

					D3D12_BARRIER_GROUP TexBarrierGroups[] =
					{
						CD3DX12_BARRIER_GROUP(1,TexBarries)
					};

					copyWorkload.commandList7->Barrier(1, TexBarrierGroups);
				
				}

				SubmitCopyWorkload(copyWorkload);	
#endif
			}

			//创建资源描述符的DESC
			if (EnumClassHasFlags(desc->flags, RenderBackendTextureCreateFlags::ShaderResource)) 
			{
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = texture->format;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				
				switch (srvDesc.Format) 
				{
				case DXGI_FORMAT_D16_UNORM:
					srvDesc.Format = DXGI_FORMAT_R16_UNORM;
					break;
				case DXGI_FORMAT_D32_FLOAT:
					srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
					break;
				case DXGI_FORMAT_D24_UNORM_S8_UINT:
					srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
					break;
				case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
					srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
					break;

				}
				switch (desc->type) 
				{
				case RenderBackendTextureType::Texture1D:
				{
					if (texture->arraySize == 1)
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
						srvDesc.Texture1D.MostDetailedMip = 0;
						srvDesc.Texture1D.MipLevels = texture->mipLevels;
						srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
					}
					else
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
						srvDesc.Texture1DArray.FirstArraySlice = 0;
						srvDesc.Texture1DArray.ArraySize = texture->arraySize;
						srvDesc.Texture1DArray.MostDetailedMip = 0;
						srvDesc.Texture1DArray.MipLevels = texture->mipLevels;
						srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
					}
				}break;
				case RenderBackendTextureType::Texture2D: 
				{
					uint32 planeSlice = 0;
					if (texture->arraySize == 1)
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						srvDesc.Texture2D.MostDetailedMip = 0; //从mipmap0开始访问
						srvDesc.Texture2D.MipLevels = texture->mipLevels;
						srvDesc.Texture2D.PlaneSlice = planeSlice; //如果是深度-模板缓冲，0表示访问的是深度数据，1表示的是访问模板数据
						srvDesc.Texture2D.ResourceMinLODClamp = 0.0f; //shader能访问的mipmap级别，0.0f表示能访问所有mipmap级别， 2.0f表示只能访问2以及以上的mipmap级别
					}
					else
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						srvDesc.Texture2DArray.FirstArraySlice = 0; //指从纹理数组的哪个纹理开始访问
						srvDesc.Texture2DArray.ArraySize = texture->arraySize;
						srvDesc.Texture2DArray.MostDetailedMip = 0;
						srvDesc.Texture2DArray.MipLevels = texture->mipLevels;
						srvDesc.Texture2DArray.PlaneSlice = planeSlice;
						srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
					}
				}break;
				case RenderBackendTextureType::Texture3D:
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
					srvDesc.Texture3D.MostDetailedMip = 0;
					srvDesc.Texture3D.MipLevels = texture->mipLevels;
					srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
				}break;
				case RenderBackendTextureType::TextureCube:
				{
					if (texture->arraySize == 6)
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
						srvDesc.TextureCube.MostDetailedMip = 0;
						srvDesc.TextureCube.MipLevels = texture->mipLevels;
						srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
					}
					else
					{
						srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
						srvDesc.TextureCubeArray.MostDetailedMip = 0;
						srvDesc.TextureCubeArray.MipLevels = texture->mipLevels;
						srvDesc.TextureCubeArray.First2DArrayFace = 0;//从第一个立方体贴图的第一个面开始读取数据
						srvDesc.TextureCubeArray.NumCubes = texture->arraySize / 6;
						srvDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
					}
				}break;
				}
				
				{
					//创建资源描述符
					texture->shaderResourceView = new D3D12ShaderResourceView();
					//先临时分配一个资源描述符句柄
					texture->shaderResourceView->descriptor = resourceDescriptorAllocator.Allocate();
					device->CreateShaderResourceView(texture->GetID3D12Resource(), &srvDesc, texture->shaderResourceView->descriptor);
					//获取bindless index
					texture->shaderResourceView->bindlessIndex = AllocateResourceDescriptorIndex();
					if (texture->shaderResourceView->bindlessIndex >= 0)
					{
						assert(texture->shaderResourceView->bindlessIndex < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
						D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
						rangeStart.ptr += texture->shaderResourceView->bindlessIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
						//将获取的临时资源描述符句柄拷贝到bindless句柄中
						device->CopyDescriptorsSimple(1, rangeStart, texture->shaderResourceView->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					}
				}

				//给每个mipmap生成描述符
				texture->shaderResourceViews.resize(texture->mipLevels);
				for (uint32 mipLevel = 0; mipLevel < texture->mipLevels; mipLevel++)
				{
					switch (desc->type)
					{
					case RenderBackendTextureType::Texture1D:
					{
						if (texture->arraySize == 1)
						{
							srvDesc.Texture1D.MostDetailedMip = mipLevel;
							srvDesc.Texture1D.MipLevels = 1;
						}
						else
						{
							srvDesc.Texture1DArray.MostDetailedMip = mipLevel;
							srvDesc.Texture1D.MipLevels = 1;
						}
					}break;
					case RenderBackendTextureType::Texture2D:
					{
						uint32 planeSlice = 0;
						if (texture->arraySize == 1)
						{
							srvDesc.Texture2D.MostDetailedMip = mipLevel;
							srvDesc.Texture2D.MipLevels = 1;
						}
						else
						{
							srvDesc.Texture2DArray.MostDetailedMip = mipLevel;
							srvDesc.Texture2DArray.MipLevels = 1;
						}
					}break;
					case RenderBackendTextureType::Texture3D:
					{
						srvDesc.Texture3D.MostDetailedMip = mipLevel;
						srvDesc.Texture3D.MipLevels = 1;
					}break;
					case RenderBackendTextureType::TextureCube:
					{
						if (texture->arraySize == 6)
						{
							srvDesc.TextureCube.MostDetailedMip = mipLevel;
							srvDesc.TextureCube.MipLevels = 1;
						}
						else
						{
							srvDesc.TextureCubeArray.MostDetailedMip = mipLevel;
							srvDesc.TextureCubeArray.MipLevels = 1;
						}
					}break;
					}

					texture->shaderResourceViews[mipLevel] = new D3D12ShaderResourceView();
					//先临时分配一个资源描述符句柄
					texture->shaderResourceViews[mipLevel]->descriptor = resourceDescriptorAllocator.Allocate();
					device->CreateShaderResourceView(texture->GetID3D12Resource(), &srvDesc, texture->shaderResourceViews[mipLevel]->descriptor);

					texture->shaderResourceViews[mipLevel]->bindlessIndex = AllocateResourceDescriptorIndex();
					if (texture->shaderResourceViews[mipLevel]->bindlessIndex >= 0) 
					{
						assert(texture->shaderResourceViews[mipLevel]->bindlessIndex < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
						D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
						rangeStart.ptr += texture->shaderResourceViews[mipLevel]->bindlessIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
						device->CopyDescriptorsSimple(1, rangeStart, texture->shaderResourceViews[mipLevel]->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					}

				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendTextureCreateFlags::RenderTarget))
			{
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
                rtvDesc.Format = texture->format;
				
                texture->renderTargetViews.resize(texture->mipLevels);
				for (uint32 mipSlice = 0; mipSlice < texture->mipLevels; mipSlice++) 
				{
					{
						switch (desc->type) 
						{
						case RenderBackendTextureType::Texture1D:
						{
							if (texture->arraySize == 1)
							{
								rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
								rtvDesc.Texture1D.MipSlice = mipSlice;
							}
							else
							{
								rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
								rtvDesc.Texture1DArray.FirstArraySlice = 0;
								rtvDesc.Texture1DArray.ArraySize = texture->arraySize;
								rtvDesc.Texture1DArray.MipSlice = mipSlice;
							}
						}break;
						case RenderBackendTextureType::Texture2D: 
						{
							if (texture->arraySize == 1)
							{
								rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
								rtvDesc.Texture2D.MipSlice = mipSlice;
								rtvDesc.Texture2D.PlaneSlice = 0;
							}
							else
							{
								rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
								rtvDesc.Texture2DArray.FirstArraySlice = 0;
								rtvDesc.Texture2DArray.ArraySize = texture->arraySize;
								rtvDesc.Texture2DArray.MipSlice = mipSlice;
								rtvDesc.Texture2DArray.PlaneSlice = 0;

							}
						}break;
						case RenderBackendTextureType::Texture3D: 
						{
							rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
							rtvDesc.Texture3D.MipSlice = mipSlice;
							rtvDesc.Texture3D.FirstWSlice = 0;
							rtvDesc.Texture3D.WSize = -1;

						}break;
						}

						uint32 rtvIndex = mipSlice;
						texture->renderTargetViews[rtvIndex] = new D3D12RenderTargetView();
						texture->renderTargetViews[rtvIndex]->descriptor = rtvDescriptorAllocator.Allocate();
						device->CreateRenderTargetView(texture->GetID3D12Resource(), &rtvDesc, texture->renderTargetViews[rtvIndex]->descriptor);
					}
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendTextureCreateFlags::DepthStencil)) 
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = texture->format;
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				
				switch (desc->type) 
				{
				case RenderBackendTextureType::Texture1D:
				{
					if (texture->arraySize == 1)
					{
						dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
						dsvDesc.Texture1D.MipSlice = 0;
					}
					else
					{
						dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
						dsvDesc.Texture1DArray.FirstArraySlice = 0;
						dsvDesc.Texture1DArray.ArraySize = texture->arraySize;
						dsvDesc.Texture1DArray.MipSlice = 0;
					}
				}break;
				case RenderBackendTextureType::Texture2D:
				{
					if (texture->arraySize == 1)
					{
						dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
						dsvDesc.Texture2D.MipSlice = 0;
					}
					else
					{
						dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
						dsvDesc.Texture2DArray.FirstArraySlice = 0;
						dsvDesc.Texture2DArray.ArraySize = texture->arraySize;
						dsvDesc.Texture2DArray.MipSlice = 0;
					}
				}break;
				}

				texture->depthStencilViews[0] = new D3D12DepthStencilView();
				texture->depthStencilViews[0]->descriptor = dsvDescriptorAllocator.Allocate();
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
				device->CreateDepthStencilView(texture->GetID3D12Resource(), &dsvDesc, texture->depthStencilViews[0]->descriptor);

				texture->depthStencilViews[1] = new D3D12DepthStencilView();
				texture->depthStencilViews[1]->descriptor = dsvDescriptorAllocator.Allocate();
				dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
				device->CreateDepthStencilView(texture->GetID3D12Resource(), &dsvDesc, texture->depthStencilViews[1]->descriptor);
			
				const bool hasStencil = false;
				if (hasStencil)
				{
					texture->depthStencilViews[2] = new D3D12DepthStencilView();
					texture->depthStencilViews[2]->descriptor = dsvDescriptorAllocator.Allocate();
					dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
					device->CreateDepthStencilView(texture->GetID3D12Resource(), &dsvDesc, texture->depthStencilViews[2]->descriptor);

					texture->depthStencilViews[3] = new D3D12DepthStencilView();
					texture->depthStencilViews[3]->descriptor = dsvDescriptorAllocator.Allocate();
					dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
					device->CreateDepthStencilView(texture->GetID3D12Resource(), &dsvDesc, texture->depthStencilViews[3]->descriptor);
				}
			}

			if (EnumClassHasFlags(desc->flags, RenderBackendTextureCreateFlags::UnorderedAccess)) 
			{
				D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
				uavDesc.Format = texture->format;
			
				texture->unorderedAccessViews.resize(texture->mipLevels);
				for (uint32 mipSlice = 0; mipSlice < texture->mipLevels; mipSlice++) 
				{
					switch (desc->type) 
					{
					case RenderBackendTextureType::Texture1D: 
					{
						if (texture->arraySize == 1)
						{
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
							uavDesc.Texture1D.MipSlice = mipSlice;
						}
						else
						{
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
							uavDesc.Texture1DArray.FirstArraySlice = 0;
							uavDesc.Texture1DArray.ArraySize = texture->arraySize;
							uavDesc.Texture1DArray.MipSlice = mipSlice;

						}

					}break;
					case RenderBackendTextureType::Texture2D:
					case RenderBackendTextureType::TextureCube:
					{
						if (texture->arraySize == 1)
						{
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
							uavDesc.Texture2D.MipSlice = mipSlice;
							uavDesc.Texture2D.PlaneSlice = 0;
						}
						else
						{
							uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
							uavDesc.Texture2DArray.FirstArraySlice = 0;
							uavDesc.Texture2DArray.ArraySize = texture->arraySize;
							uavDesc.Texture2DArray.MipSlice = mipSlice;
							uavDesc.Texture2DArray.PlaneSlice = 0;

						}
					}break;
					case RenderBackendTextureType::Texture3D:
					{
                        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                        uavDesc.Texture3D.MipSlice = mipSlice;//使用的mipmap级别
                        uavDesc.Texture3D.FirstWSlice = 0;//从第0个深度切片开始读取数据
                        uavDesc.Texture3D.WSize = -1;//使用所有深度切片
					}break;
					}

					texture->unorderedAccessViews[mipSlice] = new D3D12UnorderedAccessView();
					texture->unorderedAccessViews[mipSlice]->descriptor = resourceDescriptorAllocator.Allocate();
					device->CreateUnorderedAccessView(texture->GetID3D12Resource(), nullptr, &uavDesc, texture->unorderedAccessViews[mipSlice]->descriptor);
					
					texture->unorderedAccessViews[mipSlice]->bindlessIndex = AllocateResourceDescriptorIndex();
					if (texture->unorderedAccessViews[mipSlice]->bindlessIndex >= 0)
					{
						assert(texture->unorderedAccessViews[mipSlice]->bindlessIndex < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS);
						D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = resourceDescriptorHeap->cpuDescriptorHandle;
						rangeStart.ptr += texture->unorderedAccessViews[mipSlice]->bindlessIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
						device->CopyDescriptorsSimple(1, rangeStart, texture->unorderedAccessViews[mipSlice]->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					}
				}
			}
			return index;
		}

		uint32 AllocateSampler()
		{
			uint32 samplerIndex = 0;
			if (!freeSamplers.empty()) 
			{
				samplerIndex = freeSamplers.back();
				freeSamplers.pop_back();
			}
			else 
			{
				samplerIndex = (uint32)samplers.size();
                samplers.emplace_back(new D3D12Sampler());
            
            }
            return samplerIndex;
		}

		uint32 AllocateRayTracingAccelerationStructure()
		{
			uint32 index = 0;
			if (!freeAccelerationStructures.empty())
			{
				index = freeAccelerationStructures.back();
				freeAccelerationStructures.pop_back();
			}
			else
			{
				index = (uint32)accelerationStructures.size();
				accelerationStructures.emplace_back(new D3D12RayTracingAccelerationStructure());
			}
			return index;
		}

		uint32 AllocateShader() 
		{
			uint32 shaderIndex = 0;
			if (!freeShaders.empty())
			{
				shaderIndex = freeShaders.back();
				freeShaders.pop_back();	
			}
			else 
			{
				shaderIndex = (uint32)shaders.size();
				shaders.emplace_back(new D3D12Shader());
			}
			return shaderIndex;
		}

		uint32 CreateD3D12Sampler(const RenderBackendSamplerDesc* desc, const char* name)
		{
			uint32 index = AllocateSampler();
			D3D12Sampler* sampler = samplers[index];
			
			D3D12_SAMPLER_DESC samplerDesc = 
			{
				ConvertToD3D12Filter(desc->filter),
				ConvertToD3D12TextureAddressMode(desc->addressModeU),
				ConvertToD3D12TextureAddressMode(desc->addressModeV),
				ConvertToD3D12TextureAddressMode(desc->addressModeW),
				desc->mipLodBias,
				desc->maxAnisotropy,
				ConvertToD3D12ComparisonFunc(desc->compareOp),
				{0.0f, 0.0f, 0.0f, 0.0f},
				desc->minLod,
				desc->maxLod
			};

			sampler->descriptor = samplerDescriptorAllocator.Allocate();
            device->CreateSampler(&samplerDesc, sampler->descriptor);
			
			sampler->bindlessIndex = AllocateSamplerDescriptorIndex();
			if (sampler->bindlessIndex >= 0)
			{
				assert(sampler->bindlessIndex < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_SAMPLER_DESCRIPTORS);
				D3D12_CPU_DESCRIPTOR_HANDLE rangeStart = samplerDescriptorHeap->cpuDescriptorHandle;
				rangeStart.ptr += sampler->bindlessIndex * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
				device->CopyDescriptorsSimple(1, rangeStart, sampler->descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			}


			return index;
		}

		inline void AllocateUAVBuffer(
			ID3D12Device* pDevice,
			UINT64 bufferSize,
			ID3D12Resource** ppResource,
			D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON,
			const wchar_t* resourceName = nullptr)
		{
			auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize,D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE);

			pDevice->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				initialResourceState,
				nullptr,
				IID_PPV_ARGS(ppResource)
			);
			if (resourceName) 
			{
                (*ppResource)->SetName(resourceName);
			}
		}

		inline void AllocateUploadBuffer(
			ID3D12Device* pDevice,
			void* pData,
			UINT64 dataSize,
			ID3D12Resource** ppResource,
			const wchar_t* resourceName = nullptr
		) 
		{
			auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);
			//上传堆的资源位于cpu中
			pDevice->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,//堆类型
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,//初始状态，决定GPU如何访问此资源
				nullptr,
				IID_PPV_ARGS(ppResource)
			);
            if (resourceName)
            {
                (*ppResource)->SetName(resourceName);
            }
			void* pMappedData = nullptr;

			// 明确表示不读取（更规范）
			//D3D12_RANGE readRange = { 0, 0 };  // 读取0字节

			(*ppResource)->Map(
				0, // 子资源索引（缓冲区只有一个，填0）
				nullptr, // 读取范围（nullptr = 不读取）
				&pMappedData// 输出：CPU可写的指针
			);
			
			memcpy(pMappedData, pData, dataSize);
			
			// 明确写入范围（优化）
			//D3D12_RANGE writeRange = { 0, dataSize };
			(*ppResource)->Unmap(
				0,   // 子资源索引
				nullptr // 写入范围（nullptr = 全部）
			);
		}
		
		//todo
		//uint32 CreateD3D12RayTracingBottomLevelAccelerationStructure(const RenderBackendRayTracingBottomLevelAccelerationStructureDesc* desc, const char* name)

		void InitD3D12BlendDesc(const RenderBackendColorBlendStateDescription& blendState, uint32 numRenderTargets, D3D12_BLEND_DESC& dstBlendState)
		{
			dstBlendState = {};
			dstBlendState.AlphaToCoverageEnable = FALSE; //AlphaToCoverageEnable是一种多重采样抗锯齿（MSAA）技术，用于半透明物体的边缘平滑。
			dstBlendState.IndependentBlendEnable = TRUE; //控制是否允许每个渲染目标（Render Target）使用不同的混合设置。
			
			for (uint32 i = 0; i < numRenderTargets; i++) 
			{
				dstBlendState.RenderTarget[i].BlendEnable = blendState.targetBlends->blendEnable;
				dstBlendState.RenderTarget[i].LogicOpEnable = FALSE;
				dstBlendState.RenderTarget[i].SrcBlend = ConvertToD3D12Blend(blendState.targetBlends->srcColorBlendFactor);
				dstBlendState.RenderTarget[i].DestBlend = ConvertToD3D12Blend(blendState.targetBlends->dstColorBlendFactor);
				dstBlendState.RenderTarget[i].BlendOp = ConvertToD3D12BlendOp(blendState.targetBlends->colorBlendOp);
				dstBlendState.RenderTarget[i].SrcBlendAlpha = ConvertToD3D12Blend(blendState.targetBlends->srcAlphaBlendFactor);
				dstBlendState.RenderTarget[i].DestBlendAlpha = ConvertToD3D12Blend(blendState.targetBlends->dstAlphaBlendFactor);
				dstBlendState.RenderTarget[i].BlendOpAlpha = ConvertToD3D12BlendOp(blendState.targetBlends->alphaBlendOp);
				dstBlendState.RenderTarget[i].LogicOp = D3D12_LOGIC_OP_CLEAR;
				dstBlendState.RenderTarget[i].RenderTargetWriteMask = ConvertToD3D12RenderTargetWriteMask(blendState.targetBlends->writeMask);
			}
		}

		void InitD3D12RasterizerDesc(const RenderBackendRasterizationStateDescription& rasterizationState, D3D12_RASTERIZER_DESC& dstRasterizationState)
		{
			dstRasterizationState = {};
			dstRasterizationState.FillMode = ConvertToD3D12FillMode(rasterizationState.fillMode);
			dstRasterizationState.CullMode = ConvertToD3D12CullMode(rasterizationState.cullMode);
			dstRasterizationState.FrontCounterClockwise = rasterizationState.frontFaceCounterClockwise; //正面定义，默认顺时针是正面,True - 逆时针为正面，False - 顺时针为正面
			//深度偏移
			/**
			最终偏移 = depthBiasConstantFactor × r + depthBiasSlopeFactor × maxSlope
			r = 深度缓冲区能表示的最小差值
			maxSlope = max(|dz/dx|, |dz/dy|)
			其中：
			dz/dx = 深度值在屏幕X方向的变化率
			dz/dy = 深度值在屏幕Y方向的变化率
			**/
			dstRasterizationState.DepthBias = (int32)rasterizationState.depthBiasConstantFactor; //深度偏移常量系数，
			dstRasterizationState.DepthBiasClamp = rasterizationState.depthBiasClamp; //深度偏移限制在[-depthBiasClamp,depthBiasClamp]
			dstRasterizationState.SlopeScaledDepthBias = rasterizationState.depthBiasSlopeFactor; //深度斜面偏移系数
			
			dstRasterizationState.DepthClipEnable = !rasterizationState.depthClampEnable;//深度裁剪，如果为true,超过深度范围的[0,1.0]的直接丢弃，如果为false,映射到0-1.0
			dstRasterizationState.MultisampleEnable = FALSE;//是否开启多重采样
			dstRasterizationState.AntialiasedLineEnable = FALSE;//是否开启线条抗锯齿
			dstRasterizationState.ForcedSampleCount = 0;//为0时,光栅化采样数 = Rendertarget的采样数，不为0时，光栅化采样数等于设置的数值
			dstRasterizationState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; //保守光栅化：只要三角形触及到像素就光栅化，标准光栅化，像素的中心点在三角形内部才光栅化

		}

		void InitD3D12DepthStencilDesc(const RenderBackendDepthStencilStateDescription& depthStencilState, D3D12_DEPTH_STENCIL_DESC& dstDepthStencilState) 
		{
			dstDepthStencilState = {};
			dstDepthStencilState.DepthEnable = depthStencilState.depthTestEnable;
			dstDepthStencilState.DepthWriteMask = depthStencilState.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			dstDepthStencilState.DepthFunc = ConvertToD3D12ComparisonFunc(depthStencilState.depthCompareFunction);
			dstDepthStencilState.StencilEnable = depthStencilState.stencilTestEnable;
			dstDepthStencilState.StencilReadMask = (UINT8)depthStencilState.stencilReadMask;
			dstDepthStencilState.StencilWriteMask = (UINT8)depthStencilState.stencilWriteMask;
			
			dstDepthStencilState.FrontFace.StencilFailOp = ConvertToD3D12StencilOp(depthStencilState.frontFaceStencilFailOp);
			dstDepthStencilState.FrontFace.StencilDepthFailOp = ConvertToD3D12StencilOp(depthStencilState.frontFaceStencilDepthFailOp);
			dstDepthStencilState.FrontFace.StencilPassOp = ConvertToD3D12StencilOp(depthStencilState.frontFaceStencilPassOp);
			dstDepthStencilState.FrontFace.StencilFunc = ConvertToD3D12ComparisonFunc(depthStencilState.frontFaceStencilCompareFunction);

			dstDepthStencilState.BackFace.StencilFailOp = ConvertToD3D12StencilOp(depthStencilState.backFaceStencilFailOp);
			dstDepthStencilState.BackFace.StencilDepthFailOp = ConvertToD3D12StencilOp(depthStencilState.backFaceStencilDepthFailOp);
			dstDepthStencilState.BackFace.StencilPassOp = ConvertToD3D12StencilOp(depthStencilState.backFaceStencilPassOp);
			dstDepthStencilState.BackFace.StencilFunc = ConvertToD3D12ComparisonFunc(depthStencilState.backFaceStencilCompareFunction);
		
		}

		uint32 CreateD3D12Shader(const RenderBackendShaderDesc* desc, const char* name) 
		{
			uint32 index = AllocateShader();

			D3D12Shader* shader = shaders[index];

			shader->bytecode.pShaderBytecode = desc->code;
			shader->bytecode.BytecodeLength = desc->codeSize;
			shader->entryFunctionName = D3D12Utils::Widen(desc->entryFunctionName);
			shader->hash = CRC32(desc->code, desc->codeSize);
			
			return index;
		}

		void Tick() 
		{
			//这里为什么用指针的引用？应为可以直接修改原指针的指向的地址
			//如果不加引用，那么就是简单复制一个指向相同的地址的指针而已
			//这个时候workload = nullptr 只是将复制的指针置空
			for (D3D12SubmissionWorkload*& workload : workloads) 
			{
				if (workload->commandQueue->fence->GetCompletedValue() >= workload->completionFenceValue)
				{
					for (D3D12CommandAllocator* commandAllocator : workload->commandAllocatorsToRelease) 
					{
						ReleaseCommandAllocator(commandAllocator);
					}
					delete workload;
					workload = nullptr;

				}
			
			}
			workloads.erase(std::remove(workloads.begin(), workloads.end(), nullptr), workloads.end());
		}

		bool Init(D3D12RenderBackend* backend, D3D12Adapter* adapter);
		void Exit();
	

		//用于将数据上传到上传堆，在从上传堆复制到默认堆
		CopyWorkload AllocateCopyWorkload(uint64 bufferSize)
		{
			CopyWorkload workload;

			for (uint32 i = 0; i < copyWorkLoadFreeList.size(); i++)
			{
				if ((copyWorkLoadFreeList[i].uploadBuffer != nullptr) && (copyWorkLoadFreeList[i].uploadBuffer->size >= bufferSize))
				{
					{
						D3D12_CHECK(copyWorkLoadFreeList[i].fence->Signal(0));
						workload = std::move(copyWorkLoadFreeList[i]);
						std::swap(copyWorkLoadFreeList[i], copyWorkLoadFreeList.back());
						copyWorkLoadFreeList.pop_back();
						return workload;
					}
				}
			}

			D3D12_CHECK(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&workload.commandAllocator)));
			D3D12_CHECK(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, workload.commandAllocator.Get(), nullptr, IID_PPV_ARGS(&workload.commandList)));
			D3D12_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&workload.fence)));

			D3D12_CHECK(workload.commandList->QueryInterface(IID_PPV_ARGS(&workload.commandList7)));

			D3D12_CHECK(workload.commandList->Close());

			RenderBackendBufferDescription uploadBufferDesc = RenderBackendBufferDescription::CreateUpload(bufferSize);
			uint32 uploadBufferIndex = CreateD3D12Buffer(&uploadBufferDesc, nullptr, "CopyWorkload_UploadBuffer");
			workload.uploadBuffer = buffers[uploadBufferIndex];

			return workload;
		}


		void WaitIdle() const
		{
#if 1
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;

			//创建围栏
			D3D12_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
			//设置围栏初始值
			D3D12_CHECK(fence->Signal(0));
			for (const auto& commandQueue : commandQueues)
			{
				if (commandQueue)
				{
					//向命令队列添加Sigal命令，设置fence的值为1(这个命令在GPU中执行),CPU中fence的值还是0
					D3D12_CHECK(commandQueue->GetID3D12CommandQueue()->Signal(fence.Get(), 1));
					//在CPU侧检测值是不是小于1
					if (fence->GetCompletedValue() < 1)
					{
						//如果小于则开始等待，直到fence的值为1，并设置fence的值为1
						D3D12_CHECK(fence->SetEventOnCompletion(1, NULL));
					}
					//设置fence的值为1
					D3D12_CHECK(fence->Signal(0));
				}

			}
#endif // 1
		}


		void SubmitCopyWorkload(const CopyWorkload& workload) 
		{
			workload.commandList->Close();
			ID3D12CommandList* commandlists[] = { workload.commandList.Get()};
			workload.commandList.GetAddressOf();

			GetCommandQueue(D3D12CommandQueueType::Direct)->GetID3D12CommandQueue()->ExecuteCommandLists(1, commandlists);

			WaitIdle();

			copyWorkLoadFreeList.push_back(workload);
		}
		
		D3D12ComputePipelineState* FindOrCreateComputePipelineState(D3D12Shader* shader) 
		{
			uint32 shaderHash = shader->hash;
			uint64 pipelineStateHash = shaderHash;
			if (computePipelineStateMap.find(pipelineStateHash) != computePipelineStateMap.end()) 
			{
				return computePipelineStateMap[pipelineStateHash];
			}
			
			D3D12ComputePipelineState* newComputePipelineState = new D3D12ComputePipelineState();

			D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc =
			{
				rootSignature.Get(),
				shader->bytecode,
				mask.Get(), //节点掩码,指定在哪个节点上创建Pso
                nullptr, //CachedPSO,缓存的Pso ,nullptr表示不使用缓存
				D3D12_PIPELINE_STATE_FLAG_NONE /**D3D12_PIPELINE_STATE_FLAG_NONE:无特殊标志（最常用）                        
											   D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG:启用工具调试支持**/
			};

			
			D3D12_CHECK(device->CreateComputePipelineState(&computePipelineStateDesc,IID_PPV_ARGS(&newComputePipelineState->state)));
			computePipelineStateMap.emplace(pipelineStateHash, newComputePipelineState);
			return computePipelineStateMap[pipelineStateHash];
		}

		D3D12GraphicsPipelineState* FindOrCreateGraphicsPipelineState(
			D3D12Shader* vertexShader,
			D3D12Shader* pixelShader,
			D3D12Shader* amplificationShader,
			D3D12Shader* meshShader,
			const RenderBackendGraphicsPipelineStateDescription& pipelineState,
			D3D12RenderPass* renderPass,
			RenderBackendPrimitiveTopology topology
		)
		{
			bool useMeshShader = meshShader != nullptr;

			D3D12GraphicsPipelineStateDesc pipelineStateDesc = {};
			InitD3D12RasterizerDesc(pipelineState.rasterizationState, pipelineStateDesc.rasterizerState);
			InitD3D12DepthStencilDesc(pipelineState.depthStencilState, pipelineStateDesc.depthStencilState);
			InitD3D12BlendDesc(pipelineState.colorBlendState, renderPass->numRenderTargets, pipelineStateDesc.blendState);

			uint64 pipelineStateDescHash = CRC32(&pipelineStateDesc, sizeof(pipelineStateDesc));
			uint32 renderPassHash = CRC32(renderPass, sizeof(D3D12RenderPass));

			uint32 vertexShaderHash = vertexShader ? vertexShader->hash : 0;
			uint32 pixelShaderHash = pixelShader ? pixelShader->hash : 0;
			uint32 amplificationShaderHash = amplificationShader ? amplificationShader->hash : 0;
			uint32 meshShaderHash = meshShader ? meshShader->hash : 0;

			uint64 values[] = { uint64(vertexShaderHash), uint64(pixelShaderHash), uint64(amplificationShaderHash), uint64(meshShaderHash), uint64(topology), pipelineStateDescHash };
			uint32 psoHash = CRC32(values, ArraySize(values) * sizeof(UINT64));

			uint64 pipelineStateHash = (uint64(psoHash) << 32) | uint64(renderPass);

			if (graphicsPipelineStateMap.find(pipelineStateHash) != graphicsPipelineStateMap.end())
			{
                return graphicsPipelineStateMap[pipelineStateHash];
				
			}

			D3D12GraphicsPipelineState* newGraphicsPipelineState = new D3D12GraphicsPipelineState();

			D3D12_INPUT_LAYOUT_DESC nullInputLayout = { nullptr, 0 };

			D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType = ConvertToD3D12PrimitiveTopologyType(topology);

			newGraphicsPipelineState->primitiveTopologyType = primitiveTopologyType;

			DXGI_SAMPLE_DESC dxgiSampleDesc =
			{
				1,//每个像素的采样数
				0 //质量级别，质量级别越高，采样点分布越均匀
			};

			if (useMeshShader) 
			{
				ID3D12Device2* device2 = nullptr;
				assert(SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&device2))));//查询是否支持device2

				D3DX12_MESH_SHADER_PIPELINE_STATE_DESC meshShaderPipelineStateDesc =
				{
					rootSignature.Get(),
					amplificationShader ? amplificationShader->bytecode : D3D12_SHADER_BYTECODE{},//放大着色器用于指派MS
					meshShader->bytecode, /**网格着色器 替代VS + HS(曲面细分的控制阶段，决定"细分多少"和"怎么细分") + 
					DS(对 Tessellator 生成的每个新顶点计算最终位置（相当于细分后的"顶点着色器"）) + 
                    GS(几何着色器，用于处理图元并生成新的图元)
					**/
					pixelShader->bytecode,
					pipelineStateDesc.blendState,
					0xFFFFFFFF,//控制 MSAA 中哪些采样点参与渲染,0xFFFFFFFF = 所有 32 位都是 1 = 所有采样点都启用
					pipelineStateDesc.rasterizerState,
					pipelineStateDesc.depthStencilState,
					primitiveTopologyType,
					renderPass->numRenderTargets,
					{},
					renderPass->hasDepthStencil ? renderPass->depthStencilViewFormat : DXGI_FORMAT_UNKNOWN,
					dxgiSampleDesc,
					mask.Get(),//GPU掩码，指定使用哪个GPU
                    nullptr,
					D3D12_PIPELINE_STATE_FLAG_NONE

				};
				
				for (uint32 i = 0; i < renderPass->numRenderTargets; i++) 
				{
					meshShaderPipelineStateDesc.RTVFormats[i] = renderPass->renderTargetFormats[i];
				}
			
				CD3DX12_PIPELINE_MESH_STATE_STREAM psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(meshShaderPipelineStateDesc);
				D3D12_PIPELINE_STATE_STREAM_DESC streamDesc =
				{
					sizeof(psoStream),
					&psoStream
				};

				D3D12_CHECK(device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&newGraphicsPipelineState->stateObject)));
			}
			else
			{
				D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc =
				{
					rootSignature.Get(),
					vertexShader->bytecode,
					pixelShader->bytecode,
					{},
					{},
					{},
					{},
					pipelineStateDesc.blendState,
					0xFFFFFFFF,//控制 MSAA 中哪些采样点参与渲染,0xFFFFFFFF = 所有 32 位都是 1 = 所有采样点都启用
					pipelineStateDesc.rasterizerState,
					pipelineStateDesc.depthStencilState,
					nullInputLayout,
					D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,//将三角带切分
					primitiveTopologyType,
					renderPass->numRenderTargets,
					{},
					renderPass->hasDepthStencil ? renderPass->depthStencilViewFormat : DXGI_FORMAT_UNKNOWN,
					dxgiSampleDesc,
					mask.Get(),
					nullptr,
					D3D12_PIPELINE_STATE_FLAG_NONE //调试相关
				};

				for (uint32 i = 0; i < renderPass->numRenderTargets; i++)
				{
					graphicsPipelineStateDesc.RTVFormats[i] = renderPass->renderTargetFormats[i];
				}

				D3D12_CHECK(device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&newGraphicsPipelineState->stateObject)));
					
			}

			graphicsPipelineStateMap.emplace(pipelineStateHash, newGraphicsPipelineState);
			return graphicsPipelineStateMap[pipelineStateHash];

		}

		//todo 
		//uint32 CreateD3D12RayTracingPipelineState(const RenderBackendRayTracingPipelineStateDesc* desc, const char* name)
		//D3D12RayTracingPipelineStateObject* GetRayTracingPipelineStateObject(RenderBackendRayTracingPipelineStateHandle handle)


	};


	class D3D12RenderBackend : public RenderBackend 
	{
	public:
		RenderBackendType GetType() const override 
		{
			return RenderBackendType::Direct3D12;
		}
	
		bool Init(const RenderBackendDesc* desc);
		void Exit();
		void Tick() override;
		void CreateRenderDevices(PhysicalDeviceID* physicalDeviceIDs, uint32 numDevices, uint32* outDeviceMasks) override;
		void DestroyRenderDevices() override;
		void FlushRenderDevices() override;
		RenderBackendDeviceContext GetNativeDevice() override;
		RenderBackendSwapChainHandle CreateSwapChain(const RenderBackendSwapChainDesc* desc)override;
		void DestroySwapChain(RenderBackendSwapChainHandle swapChain) override;
		void ResizeSwapChain(RenderBackendSwapChainHandle swapChain, uint32* width, uint32* height) override;
		bool PresentSwapChain(RenderBackendSwapChainHandle swapChain) override;
		RenderBackendTextureHandle GetActiveSwapChainBuffer(RenderBackendSwapChainHandle swapChain) override;
		RenderBackendBufferHandle CreateBuffer(const RenderBackendBufferDescription* desc, const void* data, const char* name) override;
		void ResizeBuffer(RenderBackendBufferHandle buffer, uint64 size) override;
		void MapBuffer(RenderBackendBufferHandle buffer, void** data) override;
		void UnmapBuffer(RenderBackendBufferHandle buffer) override;
		void DestroyBuffer(RenderBackendBufferHandle buffer) override;
		RenderBackendTextureHandle CreateTexture(const RenderBackendTextureDesc* desc, const void* data, const char* name) override;
		void DestroyTexture(RenderBackendTextureHandle texture) override;
		void UploadTexture(RenderBackendTextureHandle handle, const RenderBackendTextureUploadDataDesc& data) override;
		void GetTextureReadbackData(RenderBackendTextureHandle texture, void** data) override;
		RenderBackendTextureViewHandle CreateTextureView(RenderBackendTextureHandle textureHandle, const RenderBackendTextureViewDesc* desc, int32* descriptor) override;
		//RenderBackendTextureSRVHandle CreateTextureSRV(const RenderBackendTextureSRVDesc* desc, const char* name) override;
		//RenderBackendTextureUAVHandle CreateTextureUAV(const RenderBackendTextureUAVDesc* desc, const char* name) override;
		int32 GetTextureSRVBindlessResourceDescriptorIndex(RenderBackendTextureHandle srv) override;
		int32 GetTextureSRVBindlessResourceDescriptorIndex(RenderBackendTextureHandle srv, uint32 mipLevel) override;
		int32 GetTextureUAVBindlessResourceDescriptorIndex(RenderBackendTextureHandle uav, uint32 mipLevel) override;
		int32 GetBufferCBVBindlessResourceDescriptorIndex(RenderBackendBufferHandle uav) override;
		int32 GetBufferSRVBindlessResourceDescriptorIndex(RenderBackendBufferHandle uav) override;
		int32 GetBufferUAVBindlessResourceDescriptorIndex(RenderBackendBufferHandle uav) override;
		//int32 GetAccelerationStructureSRVBindlessResourceDescriptorIndex(RenderBackendRayTracingAccelerationStructureHandle accelerationStructure) override;

		RenderBackendSamplerHandle CreateSampler(const RenderBackendSamplerDesc* desc, const char* name) override;
		void DestroySampler(RenderBackendSamplerHandle sampler) override;
		RenderBackendShaderHandle CreateShader(const RenderBackendShaderDesc* desc, const char* name) override;
		void DestroyShader(RenderBackendShaderHandle shader) override;

		RenderBackendTimingQueryHeapHandle CreateTimingQueryHeap(const RenderBackendTimingQueryHeapDesc* desc, const char* name) override;
		void DestroyTimingQueryHeap(RenderBackendTimingQueryHeapHandle timingQueryHeap) override;
		void SubmitCommandLists(RenderBackendCommandList** commandLists, uint32 numCommandLists, RenderBackendSwapChainHandle swapChain) override;
		/*RenderBackendRayTracingAccelerationStructureHandle CreateRayTracingBottomLevelAccelerationStructure(const RenderBackendRayTracingBottomLevelAccelerationStructureDesc* desc, const char* name) override;
		RenderBackendRayTracingAccelerationStructureHandle CreateRayTracingTopLevelAccelerationStructure(const RenderBackendRayTracingTopLevelAccelerationStructureDesc* desc, const char* name) override;
		RenderBackendRayTracingPipelineStateHandle CreateRayTracingPipelineState(const RenderBackendRayTracingPipelineStateDesc* desc, const char* name) override;
		RenderBackendBufferHandle CreateRayTracingShaderBindingTable(const RenderBackendRayTracingShaderBindingTableDesc* desc, const char* name) override;*/

		void SetObjectName(RenderBackendTextureHandle handle, const char* name) override 
		{
			D3D12Texture* texture = devices[0]->GetTexture(handle);
			texture->GetID3D12Resource()->SetName(D3D12Utils::Widen(name).c_str());
		}

		void SetObjectName(RenderBackendBufferHandle handle, const char* name) override
		{
			D3D12Buffer* buffer = devices[0]->GetBuffer(handle);
			buffer->GetID3D12Resource()->SetName(D3D12Utils::Widen(name).c_str());
		}

		bool IsTearingSupported() const
		{
			return tearingSupported;
		}

		IDXGIFactory6* GetIDXGIFactory()
		{
			return dxgiFactory.Get();
		}
		bool useDebugLayers;
		bool useGPUBasedValidation;
		bool enableHardwareRayTracing;
		D3D12RenderBackendHandleManager handleManager;
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
		uint32 numAdapters;
		std::vector<D3D12Adapter*> adapters;

		uint32 numDevices;
		D3D12Device* devices[RenderBackendMaxDeviceCount];
		Microsoft::WRL::ComPtr<ID3D12Device> d3d12Devices[RenderBackendMaxDeviceCount];
		bool tearingSupported; //允许不等待垂直同步就呈现画面
		HMODULE dxgiLibraryHandle = NULL;
	};

	class D3D12RenderBackendCommandListContext 
	{
	public:
		D3D12RenderBackendCommandListContext(D3D12Device* device, D3D12CommandQueueType family, D3D12CommandList* commandList) 
			: device(device)
			, queueType(family)
			, activeComputePipeline(nullptr)
			, activeGraphicsPipeline(nullptr)
			//, activeRayTracingPipeline(nullptr)
			, insideRenderPass(false) {}
		virtual ~D3D12RenderBackendCommandListContext() = default;

		inline D3D12CommandQueueType GetQueueFamily() const{ return queueType; }
		inline D3D12CommandList* GetCommandList() const { return commandList; }
		bool IsAsynchronousComputeContext() const
		{
			return queueType == D3D12CommandQueueType::Compute;
		}

		bool CompileRenderBackendCommands(const RenderBackendCommandContainer& container);
		bool CompileRenderBackendCommandsAsynchronous(const RenderBackendCommandContainer& container);
		bool CompileRenderBackendCommand(const RenderBackendCommandCopyBuffer& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandCopyTexture& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandUpdateBuffer& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandUpdateTexture& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandClearBufferUAV& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandClearTextureUAV& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandBarriers& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandBeginTimingQuery& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandEndTimingQuery& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandResolveTimingQueryResults& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDispatch& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDispatchIndirect& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandSetViewport& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandSetScissor& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandSetStencilReference& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandBeginRenderPass& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandEndRenderPass& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDraw& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDrawIndirect& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDispatchMesh& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDispatchMeshIndirect& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandBeginDebugLabel& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandEndDebugLabel& command);
		//bool CompileRenderBackendCommand(const RenderBackendCommandBuildRayTracingBottomLevelAccelerationStructure& command);
		//bool CompileRenderBackendCommand(const RenderBackendCommandBuildRayTracingTopLevelAccelerationStructure& command);
		//bool CompileRenderBackendCommand(const RenderBackendCommandDispatchRays& command);
		bool CompileRenderBackendCommand(const RenderBackendCommandDispatchSuperSampling& command);

	private:
		bool PrepareForDispatch(RenderBackendShaderHandle computeShader, const RenderBackendPushConstantValues& pushConstantValues);
		bool PreapareForDraw(
			RenderBackendShaderHandle vertexShader,
			RenderBackendShaderHandle pixleShader,
			const RenderBackendGraphicsPipelineStateDescription& pipelineState,
			RenderBackendPrimitiveTopology topology,
			RenderBackendBufferHandle indexBuffer,
			const RenderBackendPushConstantValues& pushConstantValues);

		D3D12Device* device;
		D3D12CommandQueueType queueType;
		D3D12CommandList* commandList;
		D3D12RenderPass activeRenderPass;
		ID3D12PipelineState* activeComputePipeline;
		ID3D12PipelineState* activeGraphicsPipeline;
		//ID3D12StateObject* activeRayTracingPipeline;
		bool insideRenderPass;

	
	};
	

}