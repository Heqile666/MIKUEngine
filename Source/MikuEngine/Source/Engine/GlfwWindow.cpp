#include "mikupch.h"
#include "Engine/GlfwWindow.h"
#include <GLFW/glfw3.h>

namespace MIKU {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		MIKU_CORE_ERROR("GLFW Error ({0}):{1}", error, description);
	}

	// 工厂：引擎默认使用 GLFW 窗口
	Window* Window::Create(const WindowProps& props)
	{
		return new GlfwWindow(props);
	}

	GlfwWindow::GlfwWindow(const WindowProps& props)
	{
		Init(props);
	}

	GlfwWindow::~GlfwWindow()
	{
		Shutdown();
	}

	void GlfwWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		MIKU_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			MIKU_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		// 使用 RHI(DX12) 渲染，不需要 GLFW 自带的 OpenGL 上下文
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// 仅同步窗口尺寸到 m_Data（不再派发 Hazel 事件）
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
			});
	}

	void GlfwWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void GlfwWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	bool GlfwWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window) != 0;
	}

	void GlfwWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}

	bool GlfwWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}
