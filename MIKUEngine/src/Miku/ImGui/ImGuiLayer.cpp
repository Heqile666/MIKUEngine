#include "mikupch.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "Platform/DX12/ImGuiDX12Renderer.h"
#include <GLFW/glfw3.h>
#include "MIKU/Application.h"
#include "Platform/DX12/DX12Temp.h"
#include "Platform/DX12/ImGuiGLFW.h"
namespace MIKU {
	
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}
	ImGuiLayer::ImGuiLayer(std::unique_ptr<class DX12Temp>& DX12Temp)
		:Layer("ImGuiLayer")
	{
		
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

	/*	ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;*/
	
		// Simple free list based allocator
		
		

		

		DX12 = &(Application::Get().GetDX12());
		
		//ImGui_ImplDX12_Init(DX12->d3dDevice.Get(), DX12->swapChainDesc.BufferCount, DX12->swapChainDesc.BufferDesc.Format, DX12->srvHeap.Get(), DX12->srvHeap->GetCPUDescriptorHandleForHeapStart(), DX12->srvHeap->GetGPUDescriptorHandleForHeapStart());
		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device = DX12->d3dDevice.Get();
		init_info.NumFramesInFlight = DX12->swapChainDesc.BufferCount;
		init_info.RTVFormat = DX12->swapChainDesc.BufferDesc.Format;
		init_info.SrvDescriptorHeap = DX12->srvHeap.Get(); // 必须是 SHADER_VISIBLE 的 SRV Heap
		init_info.CommandQueue = DX12->cmdQueue.Get(); // 你的主渲染队列
		static ExampleDescriptorHeapAllocator g_pd3dSrvDescHeapAlloc = DX12->g_pd3dSrvDescHeapAlloc;
		init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };
		init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };
		ImGui_ImplDX12_Init(&init_info);
		ImGui_ImplGlfw_InitForOther(Application::Get().GetWindow().GetGLFWwindow(), true);
		
		
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnUpdate()
	{
		
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		

	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

}