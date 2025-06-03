#pragma once
#include"Core.h"
#include "Events/Event.h"
namespace MIKU {

	class MIKU_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		
	};

	Application* CreateApplication();


}

