#pragma once
#include "Miku/Layer.h"
#include "Miku/Events/ApplicationEvent.h"
#include "Miku/Events/KeyEvent.h"
#include "Miku/Events/MouseEvent.h"

namespace MIKU {

	class DX12Temp;
	
	class MIKU_API ImGuiLayer :public Layer {
	public:
		ImGuiLayer();
		ImGuiLayer(std::unique_ptr<DX12Temp>& DX12Temp);
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	private:
		float m_Time = 0.0f;
		DX12Temp* DX12;

	};

}
