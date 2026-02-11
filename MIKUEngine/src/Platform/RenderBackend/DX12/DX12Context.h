#pragma once

struct GLFWwindow;

namespace MIKU{

	class DX12Context {
	public:
		DX12Context(GLFWwindow* windowHandle);

		void Init();
		void SwapBuffer();

	private:
		GLFWwindow* m_WindowHandle;


	};



}