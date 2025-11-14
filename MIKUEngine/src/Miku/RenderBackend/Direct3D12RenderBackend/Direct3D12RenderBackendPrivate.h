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

#include <MIKU/Foundation/FoundationModule.h>

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
		uint32 arraySize;
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
		D3D12_BLEND_DESC blendState;
		D3D12_RASTERIZER_DESC rasterizerState;
	
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

		std::vector<D3D12TimingQueryHeap*> queryHeaps;

		struct CopyWorkload
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> commandList7;
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			D3D12Buffer* uploadBuffer;
		};
		std::vector<CopyWorkload> copyWorkLoadFreeList;

		//Bindless
		D3D12DescriptorHeap* resourceDescriptorHeap;
		D3D12DescriptorHeap* samplerDescriptorHeap;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

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

		ID3D12Device* GetID3D12Device()
		{
			return device.Get();
		}

		ID3D12Device5* GetDXRDevice()
		{
			return device5.Get();
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
				D3D12_CHECK(buffer->resource->Map(0, nullptr, &buffer->mappedData));
				// mappedData 就是CPU侧对应的映射指针，可以操作其中数据
			}
			else if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::Upload))
			{
				D3D12_RANGE readRange = { 0,0 };
				D3D12_CHECK(buffer->resource->Map(0, &readRange, &buffer->mappedData));
			}
			else
			{
				// The mapped data pointer is valid only if the resource is created with CPU access (upload or readback).
				buffer->mappedData = nullptr;
			}

			if (data != nullptr)
			{
				if (EnumClassHasFlags(desc->flags, RenderBackendBufferCreateFlags::Upload)) // Copy directly in mapped data
				{
					assert(buffer->mappedData);
					memcpy(buffer->mappedData, data, buffer->size);
				}
				else // Copy throught upload heap
				{
					CopyWorkload copyWorkload = AllocateCopyWorkload(buffer->size);
				}
			}
		}

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
		




	};

}