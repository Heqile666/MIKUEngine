#pragma once
#include "Core.h"
#include "Window.h"
#include "Miku/LayerStack.h"
#include "Miku/Events/Event.h"
#include "Miku/Events/ApplicationEvent.h"

namespace MIKU {

	class DX12Temp;

	class MIKU_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		
		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline DX12Temp& GetDX12() { return *DX12; }

	private:
		std::unique_ptr<DX12Temp> DX12;//��ͬ��Ŀ��cpp�ļ���ͷ�ļ�����ͨ������������ǰ������������cpp�ļ��а���DX12Temp
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	
	private:
		static Application* s_Instance;
		

		
	};

	Application* CreateApplication();


}

