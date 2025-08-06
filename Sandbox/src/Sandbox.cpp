#include<Miku.h>

class ExampleLayer :public MIKU::Layer {

public:
	ExampleLayer() :Layer("Example") {}
	void OnUpdate() override {
		MIKU_INFO("ExampleLayer:Update");
	}
	void OnEvent(MIKU::Event& event) override{
		MIKU_TRACE("{0}", event.ToString());
	}


};

class Sandbox :public MIKU::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new MIKU::ImGuiLayer());
	}
	~Sandbox() {
	
	}
};

MIKU::Application* MIKU::CreateApplication()
{
	return new Sandbox();
}
