#include "mikupch.h"
#include "Application.h"
#include <GLFW/glfw3.h>

namespace MIKU {

#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)


	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
	
		TestDX12();
		while (m_Running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();
			m_Window->OnUpdate();

			
		}
	}
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}


	}
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PopOverlay(layer);
	}
	void Application::TestDX12()
	{
		// 1. 检查 DX12 设备
		HRESULT hr = D3D12CreateDevice(
			nullptr,    // 默认 GPU
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&pD3DDevice)
		);
		if (FAILED(hr)) {
			printf("DX12 设备创建失败！HRESULT: 0x%X\n", hr);
			
		}
		else {
			printf("DX12 设备创建成功!");
		}

		// 2. 检查功能支持
		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels = { D3D_FEATURE_LEVEL_12_0 };
		HRESULT hr2 = pD3DDevice->CheckFeatureSupport(
			D3D12_FEATURE_FEATURE_LEVELS,
			&featureLevels,
			sizeof(featureLevels)
		);

		if (FAILED(hr2))
		{
			printf("不支持 DX12！\n");

		}

		
	}
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}