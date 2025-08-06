#include "mikupch.h"
#include "ImGuiLayer.h"
#include "imgui.h"
#include "Platform/DX12/ImGuiDX12Renderer.h"
#include <GLFW/glfw3.h>
#include "MIKU/Application.h"
namespace MIKU {
	
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}
	ImGuiLayer::ImGuiLayer(std::unique_ptr<class DX12Temp>& DX12Temp)
		:DX12Temp(DX12Temp.get()),Layer("ImGuiLayer")
	{
		
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		
		
		ImGui_ImplDX12_Init(DX12Temp->d3dDevice,DX12Temp->swapChainDesc.BufferCount,DX12Temp->swapChainDesc.BufferDesc.Format,DX12Temp->rtvHeap);
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

		ImGui_ImplDX12_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),);
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	}

}