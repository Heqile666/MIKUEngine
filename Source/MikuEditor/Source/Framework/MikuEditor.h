#pragma once
#include <memory>

namespace MIKU {

	class Window;
	class RenderContext;

	// 独立的编辑器应用（对齐 Horizon 的 HorizonEditor）：
	// 不继承任何引擎基类，自己持有窗口 / 渲染上下文 / 主循环，把引擎当库来用。
	class MikuEditor
	{
	public:
		MikuEditor();
		~MikuEditor();

		bool Init(int argc, char** argv);
		int  Run();
		void Tick();
		void Exit();

	private:
		std::unique_ptr<Window>        m_Window;
		std::unique_ptr<RenderContext> m_RenderContext;
	};

}

// 编辑器入口（由 Launcher 的 main 调用）
extern int MikuEditorMain(int argc, char** argv);
