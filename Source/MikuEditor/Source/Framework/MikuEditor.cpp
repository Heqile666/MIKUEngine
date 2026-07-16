#include "Framework/MikuEditor.h"

#include "Engine/Window.h"
#include "Engine/RenderContext.h"
#include "Foundation/Log/Log.h"

namespace MIKU {

	MikuEditor::MikuEditor() = default;
	MikuEditor::~MikuEditor() = default;

	bool MikuEditor::Init(int argc, char** argv)
	{
		Log::Init();
		MIKU_CORE_WARN("MikuEditor starting...");

		// 引擎作为库提供 Window / RenderContext
		m_Window = std::unique_ptr<Window>(Window::Create());

		m_RenderContext = std::make_unique<RenderContext>();
		m_RenderContext->Init(m_Window.get());

		return true;
	}

	int MikuEditor::Run()
	{
		// Horizon 风格主循环：轮询窗口状态直到请求关闭
		while (!m_Window->ShouldClose())
		{
			Tick();
		}
		return 0;
	}

	void MikuEditor::Tick()
	{
		m_RenderContext->BeginFrame();
		m_RenderContext->EndFrame();
		m_Window->OnUpdate();
	}

	void MikuEditor::Exit()
	{
		if (m_RenderContext)
			m_RenderContext->Shutdown();
	}

}

int MikuEditorMain(int argc, char** argv)
{
	MIKU::MikuEditor editor;
	if (!editor.Init(argc, argv))
		return -1;

	int code = editor.Run();
	editor.Exit();
	return code;
}
