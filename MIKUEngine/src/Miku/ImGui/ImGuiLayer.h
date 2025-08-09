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

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
		
	private:
	
	private:
		float m_Time = 0.0f;
		DX12Temp* DX12;

	};

}
