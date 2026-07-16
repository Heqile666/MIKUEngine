#pragma once
#include "Rendering/RenderBackend/RenderBackendInterface.h"

namespace MIKU {

	class Window;

	// 渲染上下文胶水层：封装当前项目的 RenderBackend(RHI)。
	// 目前作为“空壳”能运行：默认仅驱动窗口循环；
	// 打开 MIKU_ENABLE_RENDER_BACKEND 后接入 WIP 的 DX12 RHI（清屏/present）。
	class RenderContext
	{
	public:
		void Init(Window* window);
		void BeginFrame();
		void EndFrame();
		void Shutdown();

	private:
		RenderBackend* m_Backend = nullptr;
		RenderBackendSwapChainHandle m_SwapChain{};
	};

}
