#pragma once
#include "mikupch.h"
#include "Miku/Engine/Core/CoreDefinitions.h"

struct GLFWwindow;

namespace MIKU {

	struct WindowProps {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "MIKU Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// 精简的窗口抽象（Horizon 风格：由 app 轮询窗口状态，不再使用 Hazel 事件系统）
	class MIKU_API Window {
	public:
		virtual ~Window() {}

		virtual void OnUpdate() = 0;              // 轮询窗口事件（glfwPollEvents）
		virtual bool ShouldClose() const = 0;     // 是否收到关闭请求

		virtual void* GetNativeWindow() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}
