#include "mikupch.h"
#include "Application.h"
#include "Miku/Events/ApplicationEvent.h"
#include "Miku/Log.h"

namespace MIKU {
	Application::Application()
	{
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication)) {
			MIKU_TRACE(e.ToString());
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			MIKU_TRACE(e.ToString());
		}

		while (true) {
				
		}
	}
}