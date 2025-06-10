#pragma once
#include"Miku/Core.h"
#include "Miku/Events/Event.h"

namespace MIKU {
	class MIKU_API Layer {
	public:
		Layer(const std::string name = "Layer");
		virtual ~Layer();
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}
		 
		inline const std::string& GetName() const { return m_DebugName; }
	protected :
		std::string m_DebugName;

	};
	
}
