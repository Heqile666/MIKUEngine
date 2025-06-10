#pragma once
#include "Core.h"
#include "Window.h"
#include "Miku/LayerStack.h"
#include "Miku/Events/Event.h"
#include "Miku/Events/ApplicationEvent.h"
#include "d3d12.h"

namespace MIKU {

	class MIKU_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void TestDX12();

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
		ID3D12Device* pD3DDevice = nullptr;

		
	};

	Application* CreateApplication();


}

