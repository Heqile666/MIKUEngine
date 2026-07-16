#pragma once
#include "Engine/Window.h"

struct GLFWwindow;

namespace MIKU {

	// 引擎内置的 GLFW 窗口实现（精简版，无 Hazel 事件系统）
	class GlfwWindow : public Window
	{
	public:
		GlfwWindow(const WindowProps& props);
		virtual ~GlfwWindow();

		void OnUpdate() override;
		bool ShouldClose() const override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		inline void* GetNativeWindow() const override { return m_Window; }

	private:
		void Init(const WindowProps& props);
		void Shutdown();

	private:
		GLFWwindow* m_Window = nullptr;
		struct WindowData {
			std::string Title;
			unsigned int Width = 0, Height = 0;
			bool VSync = false;
		};
		WindowData m_Data;
	};

}
