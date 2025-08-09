#include "mikupch.h"
#include "Application.h"
#include <GLFW/glfw3.h>
#include "Platform/DX12/DX12Temp.h"
#include "Input.h"
namespace MIKU {

#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		MIKU_CORE_ASSERT(!s_Instance,"Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		DX12 = std::make_unique<DX12Temp>(m_Window);
		MIKU_CORE_ASSERT(DX12->Init(), "DX12 init failed!")
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		
		
		PushOverlay(m_ImGuiLayer);
		
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
	
		
		while (m_Running) {
			//事件产生并处理
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			//DX12渲染初始环境
			DX12->Begin();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			DX12->End();
			
			//事件轮询
			m_Window->OnUpdate();
			
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) 
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}