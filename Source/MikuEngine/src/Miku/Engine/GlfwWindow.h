#pragma once
#include "Miku/Engine/Window.h"

struct GLFWwindow;

namespace MIKU {

	// 引擎内置的 GLFW 窗口实现（脱离旧 Platform 目录）
	class GlfwWindow : public Window
	{
	public:
		GlfwWindow(const WindowProps& props);
		virtual ~GlfwWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
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
			EventCallbackFn EventCallback;
		};
		WindowData m_Data;
	};

}
