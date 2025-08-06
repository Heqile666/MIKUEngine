#pragma once
#include "Miku/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace MIKU{

	class DX12Context :public GraphicsContext {
	public:
		DX12Context(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffer() override;

	private:
		GLFWwindow* m_WindowHandle;


	};



}