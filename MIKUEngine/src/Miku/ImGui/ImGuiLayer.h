#pragma once

#include "Miku/Layer.h"
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
		float m_Time = 0.0f;
		DX12Temp* DX12;

	};

}
