#include<Miku.h>
class Sandbox :public MIKU::Application {
public:
	Sandbox() {
	
	}
	~Sandbox() {
	
	}
};

MIKU::Application* MIKU::CreateApplication()
{
	return new Sandbox();
}
