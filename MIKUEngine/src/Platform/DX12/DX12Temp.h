#pragma once
#define GLFW_EXPOSE_NATIVE_WIN32
#include "DxException.h"
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <Miku/Window.h>
#include <DirectXColors.h>
#include "GLFW\glfw3.h"
#include "GLFW\glfw3native.h"

#define ComPtr Microsoft::WRL::ComPtr 

namespace MIKU {

    class DX12Temp {
    public:
        DX12Temp() = default;
        DX12Temp(std::unique_ptr<Window>& Window) :m_Window(Window) {

        };
        ~DX12Temp() = default;
        // 设备和DXGI接口
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

        // 交换链描述符
        DXGI_SWAP_CHAIN_DESC swapChainDesc;

        D3D12_RESOURCE_DESC dsvDesc;

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;

       
    public:
        // 初始化相关
        bool Init();
        bool InitWindow(std::unique_ptr<Window>& window);
        bool InitDirect3D();

        // 运行和绘制
        void Run();
        void Draw();

        // 资源和对象创建
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

        // 命令队列同步
        void FlushCmdQueue();

    private:
       
    };

}
