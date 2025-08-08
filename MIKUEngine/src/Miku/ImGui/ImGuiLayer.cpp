#include "mikupch.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "Platform/DX12/ImGuiDX12Renderer.h"
#include <GLFW/glfw3.h>
#include "MIKU/Application.h"
#include "Platform/DX12/DX12Temp.h"
#include "Platform/DX12/ImGuiGLFW.h"


ImGuiKey ImGui_ImplGlfw_KeyToImGuiKey(int keycode, int scancode);

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
		ImGui_ImplGlfw_InitForOther((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), true);
		
		
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
		//传入事件
		EventDispatcher dispatcher(event);

		//对具体的事件进行消费
		dispatcher.Dispatch<MouseButtonPressedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.Dispatch<KeyPressedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(MIKU_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));

	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{

		//ImGui::IsKeyPressed(ImGuiKey_Space)
		//ImGuiIO& io = ImGui::GetIO();
		/*io.KeysDown[e.GetKeyCode()] = true;

		ImGui::IsKeyPressed(ImGuiKey_Space)
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];*/

		ImGui::IsKeyPressed(ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode(),e.GetScanCode()) , e.GetRepeatCount() > 0 ? true : false);
		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		/*ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;*/
		ImGui::IsKeyReleased(ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode(),e.GetScanCode()));
		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
	/*	ImGuiIO& io = ImGui::GetIO();
		int keycode = e.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);*/

		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		//glViewport(0, 0, e.GetWidth(), e.GetHeight());
		DX12 = &(Application::Get().GetDX12());
		DX12->viewPort.Height = e.GetHeight();
		DX12->viewPort.Width = e.GetWidth();
	
		DX12->scissorRect.right = e.GetWidth();
		DX12->scissorRect.bottom = e.GetHeight();
		return false;
	}


}