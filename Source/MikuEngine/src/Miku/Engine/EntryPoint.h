#pragma once
#ifdef MIKU_PLATFORM_WINDOWS

extern MIKU::Application* MIKU::CreateApplication();

int main(int argc, char** argv) {
	MIKU::Log::Init();
	MIKU_CORE_WARN("Initialized Log!");
	int a = 5;
	MIKU_INFO("Hello! Var={0}", a);
	auto sandbox = MIKU::CreateApplication();


	sandbox->Run();

	delete sandbox;

}

#endif // MIKU_PLATFORM_WINDOWS