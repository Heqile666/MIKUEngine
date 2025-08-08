#include<Miku.h>

class ExampleLayer :public MIKU::Layer {

public:
	ExampleLayer() :Layer("Example") {}
	void OnUpdate() override {
		if (MIKU::Input::IsKeyPressed(MIKU_KEY_TAB)) 
		{
			MIKU_TRACE("Tab key is pressed !");
		}
	}                                                          
	void OnEvent(MIKU::Event& event) override{
		if (event.GetEventType() == MIKU::EventType::KeyPressed)
		{
			MIKU::KeyPressedEvent& e = (MIKU::KeyPressedEvent&)event;
			if (e.GetKeyCode() == MIKU_KEY_TAB)
				MIKU_TRACE("Tab key is pressed (event)!");
				MIKU_TRACE("Tab key is pressed (event)!");
				MIKU_TRACE("{0}", (char)e.GetKeyCode());
		}
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
