#include "mikupch.h"
#include "DX12Temp.h"
#include <imgui.h>
#include <Platform/DX12/ImGuiDX12Renderer.h>
#include <Platform/DX12/ImGuiGLFW.h>

namespace MIKU {
   

    bool DX12Temp::InitWindow(std::unique_ptr<Window>& window) {
        WindowHandle = glfwGetWin32Window((GLFWwindow*)window->GetNativeWindow());
        return WindowHandle != nullptr;
    }

    bool DX12Temp::InitDirect3D() {
#if defined(DEBUG) || defined(_DEBUG)
        {
            ComPtr<ID3D12Debug> debugController;
            ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
            debugController->EnableDebugLayer();
        }
#endif

        CreateDevice();
        CreateFence();
        SetMSAA();
        CreateCommandObject();
        CreateSwapChain();
        CreateDescriptorHeap();
        GetDescriptorSize();
        CreateRTV();
        CreateDSV();
        resourceBarrierBuild();
        CreateViewPortAndScissorRect();
        g_pd3dSrvDescHeapAlloc.Create(d3dDevice.Get(), srvHeap.Get());
        return true;
    }

    bool DX12Temp::Init() {
        if (!InitWindow(m_Window))
            return false;
        if (!InitDirect3D())
            return false;
        return true;
    }

    void DX12Temp::Run() {
        Draw();
    }

    void DX12Temp::Draw() {
        ThrowIfFailed(cmdAllocator->Reset());
        ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));

        UINT& backBufferIndex = mCurrentBackBuffer;

        // 资源状态转换：Present -> Render Target
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            swapChainBuffer[backBufferIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET));

        cmdList->RSSetViewports(1, &viewPort);
        cmdList->RSSetScissorRects(1, &scissorRect);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            backBufferIndex,
            rtvDescriptorSize);

        cmdList->ClearRenderTargetView(rtvHandle, DirectX::Colors::DarkRed, 0, nullptr);

        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
        cmdList->ClearDepthStencilView(dsvHandle,
            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            1.0f, 0, 0, nullptr);

        cmdList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

        ID3D12DescriptorHeap* heaps[] = { srvHeap.Get() };
        cmdList->SetDescriptorHeaps(_countof(heaps), heaps);


        ImGui_ImplDX12_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

        // 资源状态转换：Render Target -> Present
        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            swapChainBuffer[backBufferIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT));

        ThrowIfFailed(cmdList->Close());

        ID3D12CommandList* cmdsLists[] = { cmdList.Get() };
        cmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

        ThrowIfFailed(swapChain->Present(0, 0));

        mCurrentBackBuffer = (mCurrentBackBuffer + 1) % 2;

        FlushCmdQueue();
    }

    void DX12Temp::CreateDevice() {
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
        ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3dDevice)));
    }

    void DX12Temp::CreateFence() {
        ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    }

    void DX12Temp::GetDescriptorSize() {
        rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        cbv_srv_uavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void DX12Temp::SetMSAA() {
        msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        msaaQualityLevels.SampleCount = 4;
        msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msaaQualityLevels.NumQualityLevels = 0;

        ThrowIfFailed(d3dDevice->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msaaQualityLevels,
            sizeof(msaaQualityLevels)));

        assert(msaaQualityLevels.NumQualityLevels > 0);
    }

    void DX12Temp::CreateCommandObject() {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue)));
        ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));
        ThrowIfFailed(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList)));
        // cmdList->Close(); // 这里不关闭，后续会重置
    }

    void DX12Temp::CreateSwapChain() {
        swapChain.Reset();

        swapChainDesc.BufferDesc.Width = 1280;
        swapChainDesc.BufferDesc.Height = 720;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = WindowHandle;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Windowed = TRUE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ThrowIfFailed(dxgiFactory->CreateSwapChain(cmdQueue.Get(), &swapChainDesc, swapChain.GetAddressOf()));
    }

    void DX12Temp::CreateDescriptorHeap() {
        // RTV堆
       
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

        // DSV堆
       
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        dsvHeapDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

        //SRV堆
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        srvHeapDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap))); 

    }

    void DX12Temp::CreateRTV() {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (int i = 0; i < 2; ++i) {
            ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffer[i])));
            d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    void DX12Temp::CreateDSV() {
       
        dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        dsvDesc.Alignment = 0;
        dsvDesc.Width = 1280;
        dsvDesc.Height = 720;
        dsvDesc.DepthOrArraySize = 1;
        dsvDesc.MipLevels = 1;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.SampleDesc.Count = 1;
        dsvDesc.SampleDesc.Quality = 0;
        dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        CD3DX12_CLEAR_VALUE clearValue(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0);

        ThrowIfFailed(d3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &dsvDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&depthStencilBuffer)));

        d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, dsvHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void DX12Temp::resourceBarrierBuild() {
        cmdList->Close();
        ThrowIfFailed(cmdList->Reset(cmdAllocator.Get(), nullptr));

        cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
            depthStencilBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE));

        ThrowIfFailed(cmdList->Close());

        ID3D12CommandList* cmdsLists[] = { cmdList.Get() };
        cmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    }

    void DX12Temp::FlushCmdQueue() {
        mCurrentFence++;
        ThrowIfFailed(cmdQueue->Signal(fence.Get(), mCurrentFence));

        if (fence->GetCompletedValue() < mCurrentFence) {
            HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            ThrowIfFailed(fence->SetEventOnCompletion(mCurrentFence, eventHandle));
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }

    void DX12Temp::CreateViewPortAndScissorRect() {
        viewPort.TopLeftX = 0.0f;
        viewPort.TopLeftY = 0.0f;
        viewPort.Width = 1280.0f;
        viewPort.Height = 720.0f;
        viewPort.MinDepth = 0.0f;
        viewPort.MaxDepth = 1.0f;

        scissorRect.left = 0;
        scissorRect.top = 0;
        scissorRect.right = 1280;
        scissorRect.bottom = 720;
    }


}