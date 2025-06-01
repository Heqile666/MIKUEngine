#pragma once
#include"Core.h"
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

