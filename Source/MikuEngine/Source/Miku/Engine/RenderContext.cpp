#include "mikupch.h"
#include "Miku/Engine/RenderContext.h"
#include "Miku/Engine/Window.h"

// ============================================================
//  TODO(port): 当前 Miku/RenderBackend 的 DX12 RHI 仍是开发中(WIP)，
//  其运行时（设备/交换链/present）尚未验证。默认关闭以保证空壳可运行；
//  搬运/补全 RHI 后，将下面开关置为 1 即可接入真正的清屏/present。
// ============================================================
#define MIKU_ENABLE_RENDER_BACKEND 0

#if MIKU_ENABLE_RENDER_BACKEND
#include "Miku/Rendering/RenderBackend/Direct3D12RenderBackend/Direct3D12RenderBackend.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace MIKU {

	void RenderContext::Init(Window* window)
	{
#if MIKU_ENABLE_RENDER_BACKEND
		RenderBackendDesc desc = {};
		desc.type = RenderBackendType::Direct3D12;
		desc.applicationName = "MIKU";
		desc.engineName = "MIKUEngine";
		desc.enableDebugLayer = true;

		m_Backend = RenderBackendCreateDirect3D12(&desc);

		uint32 deviceMask = 0;
		m_Backend->CreateRenderDevices(nullptr, 1, &deviceMask);

		RenderBackendSwapChainDesc scDesc = {};
		scDesc.width = window->GetWidth();
		scDesc.height = window->GetHeight();
		scDesc.windowHandle = (uint64)glfwGetWin32Window((GLFWwindow*)window->GetNativeWindow());
		scDesc.numBuffers = 3;
		scDesc.vsync = true;
		scDesc.fullScreen = false;
		m_SwapChain = m_Backend->CreateSwapChain(&scDesc);

		MIKU_CORE_INFO("RenderContext: DX12 RHI initialized.");
#else
		(void)window;
		MIKU_CORE_WARN("RenderContext: RHI disabled (window-only shell). See MIKU_ENABLE_RENDER_BACKEND.");
#endif
	}

	void RenderContext::BeginFrame()
	{
		// TODO(port): acquire back buffer / begin command list / clear
	}

	void RenderContext::EndFrame()
	{
#if MIKU_ENABLE_RENDER_BACKEND
		if (m_Backend)
		{
			m_Backend->Tick();
			m_Backend->PresentSwapChain(m_SwapChain);
		}
#endif
	}

	void RenderContext::Shutdown()
	{
#if MIKU_ENABLE_RENDER_BACKEND
		if (m_Backend)
		{
			m_Backend->DestroySwapChain(m_SwapChain);
			m_Backend->DestroyRenderDevices();
		}
#endif
	}

}
