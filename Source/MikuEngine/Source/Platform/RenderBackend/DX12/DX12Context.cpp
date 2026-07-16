#include "mikupch.h"
#include "DX12Context.h"
#include "GLFW/glfw3.h"

namespace MIKU {
	
	DX12Context::DX12Context(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		MIKU_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void DX12Context::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
	}

	void DX12Context::SwapBuffer()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}