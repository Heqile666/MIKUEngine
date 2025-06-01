#pragma once
#ifdef MIKU_PLATFORM_WINDOWS

extern MIKU::Application* MIKU::CreateApplication();

int main(int argc, char** argv) {

	auto sandbox = MIKU::CreateApplication();
	sandbox->Run();
	delete sandbox;
	
}

#endif // MIKU_PLATFORM_WINDOWS