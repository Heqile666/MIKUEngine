#pragma once
#include "RenderGraphCommon.h"
#include "RenderGraphHandles.h"
#include "RenderGraphNode.h"
#include "RenderGraphPass.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphBlackboard.h"

namespace MIKU 
{
	class RenderGraph;
	class RenderGraphPass;
	class RenderBackendGPUProfiler;

	struct RenderGraphExecuteContext 
	{
		RenderBackend* renderBackend;
		std::vector<RenderBackendCommandList*> commandList;

		RenderGraphExecuteContext() = default;
		~RenderGraphExecuteContext() 
		{
			for (RenderBackendCommandList* commandList : commandList) 
			{
				//调用对象的析构函数，但是不释放指针指向对象的内存
				std::destroy_at(commandList);
			}

		}
	};

	enum class RenderGraphSourceDataLifetimeHint
	{
		OnlyValidNow,
		ValidUntilExecution
	};

	class
}