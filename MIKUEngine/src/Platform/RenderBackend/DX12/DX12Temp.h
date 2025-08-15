#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#include "DxException.h"
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <Miku/Window.h>
#include <DirectXColors.h>
#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"
#include <imgui.h>

#define ComPtr Microsoft::WRL::ComPtr 

namespace MIKU {

    struct ExampleDescriptorHeapAllocator
    {
        ID3D12DescriptorHeap* Heap = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
        D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
        UINT                        HeapHandleIncrement;
        ImVector<int>               FreeIndices;

        void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
        {
            IM_ASSERT(Heap == nullptr && FreeIndices.empty());
            Heap = heap;
            D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
            HeapType = desc.Type;
            HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
            HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
            HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
            FreeIndices.reserve((int)desc.NumDescriptors);
            for (int n = desc.NumDescriptors; n > 0; n--)
                FreeIndices.push_back(n - 1);
        }
        void Destroy()
        {
            Heap = nullptr;
            FreeIndices.clear();
        }
        void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
        {
            IM_ASSERT(FreeIndices.Size > 0);
            int idx = FreeIndices.back();
            FreeIndices.pop_back();
            out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
            out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
        }
        void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
        {
            int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
            int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
            IM_ASSERT(cpu_idx == gpu_idx);
            FreeIndices.push_back(cpu_idx);
        }
    };

    class DX12Temp {
    public:
        DX12Temp() = default;
        DX12Temp(std::unique_ptr<Window>& Window) :m_Window(Window) {
           
        };
        ~DX12Temp() = default;
       
        ComPtr<ID3D12Device> d3dDevice;
        ComPtr<IDXGIFactory4> dxgiFactory;
        ComPtr<ID3D12Fence> fence;
        ComPtr<ID3D12CommandAllocator> cmdAllocator;
        ComPtr<ID3D12CommandQueue> cmdQueue;
        ComPtr<ID3D12GraphicsCommandList> cmdList;
        ComPtr<ID3D12Resource> depthStencilBuffer;
        ComPtr<ID3D12Resource> swapChainBuffer[2];
        ComPtr<IDXGISwapChain> swapChain;
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        ComPtr<ID3D12DescriptorHeap> dsvHeap;
        ComPtr<ID3D12DescriptorHeap> srvHeap;

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels;

        D3D12_VIEWPORT viewPort;
        D3D12_RECT scissorRect;

        UINT rtvDescriptorSize = 0;
        UINT dsvDescriptorSize = 0;
        UINT cbv_srv_uavDescriptorSize = 0;
        UINT mCurrentBackBuffer = 0;

        std::unique_ptr<Window>& m_Window;
        HWND WindowHandle = nullptr;

        int mCurrentFence = 0;

       
        DXGI_SWAP_CHAIN_DESC swapChainDesc;

        D3D12_RESOURCE_DESC dsvDesc;

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;

        ExampleDescriptorHeapAllocator g_pd3dSrvDescHeapAlloc;
       
    public:
       

        bool Init();
        bool InitWindow(std::unique_ptr<Window>& window);
        bool InitDirect3D();

        void Run();
        void Begin();
        void Draw();
        void End();

        void CreateDevice();
        void CreateFence();
        void GetDescriptorSize();
        void SetMSAA();
        void CreateCommandObject();
        void CreateSwapChain();
        void CreateDescriptorHeap();
        void CreateRTV();
        void CreateDSV();
        void CreateViewPortAndScissorRect();
        void resourceBarrierBuild();

      
        void FlushCmdQueue();

    private:
       
       
    };

}
