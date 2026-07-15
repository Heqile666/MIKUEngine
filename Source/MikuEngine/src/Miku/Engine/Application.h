#pragma once
#include "Core/CoreDefinitions.h"
#include "Window.h"
#include "Miku/Engine/LayerStack.h"
#include "Miku/Engine/Events/Event.h"
#include "Miku/Engine/Events/ApplicationEvent.h"
#include <memory>

namespace MIKU {

	class RenderContext;

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

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<RenderContext> m_RenderContext;
		bool m_Running = true;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}
