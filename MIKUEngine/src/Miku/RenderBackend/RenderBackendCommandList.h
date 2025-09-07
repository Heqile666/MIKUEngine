#pragma once
#include "Miku/Core/CoreDefinitions.h"
#include "Miku/Core/Memory/Memory.h"
#include "RenderBackendCommands.h"
#include <vector>
#include <Miku/RenderBackend/RenderBackendHandles.h>
#include <Miku/RenderBackend/RenderBackendTypes.h>
namespace MIKU 
{
	
	struct RenderBackendCommandContainer 
	{
		uint32 numCommands = 0;
		std::vector<RenderBackendCommandType> types = {};
		std::vector<void*> commands = {};
	};

	class RenderBackendCommandListBase 
	{
	public:
		RenderBackendCommandListBase(MemoryArena* arena) :arena(arena) {}
		virtual ~RenderBackendCommandListBase() {}

		template <typename T>
		FORCEINLINE T* AllocatedCommand(RenderBackendCommandType type, uint64 size = sizeof(T)) 
		{
			void* data = AllocatedCommandInternal(size);
			container.types.push_back(type);
			container.commands.push_back(data);
			container.numCommands++;
			return (T*)data;
		}
		FORCEINLINE RenderBackendCommandContainer* GetCommandContainer() 
		{
			return &container;
		}
	private:
		MemoryArena* arena;
		RenderBackendCommandContainer container;

		FORCEINLINE void* AllocatedCommandInternal(uint64 size) 
		{
			void* data = MIKU_ARENA_ALLOC(arena, size);
			return data;
		}

	
	};

	class RenderBackendCommandList : public RenderBackendCommandListBase 
	{
	public:
		RenderBackendCommandList(MemoryArena* arena) :RenderBackendCommandListBase(arena) {}

		//Copy Commands
		void CopyTexture2D(RenderBackendTextureHandle srcTexture, const Offset2D& srcOffset, uint32 srcMipLevel, RenderBackendTextureHandle dstTexture, const Offset2D& dstOffset, uint32 dstMipLevel, const Extent2D extent);
		void CopyBuffer(RenderBackendBufferHandle srcBuffer, uint64 srcOffset, RenderBackendBufferHandle dstBuffer, uint64 dstOffset, uint64 bytes);
		void UpdateBuffer(RenderBackendBufferHandle buffer, uint64 offset, const void* data, uint64 size);

		//Compute Commands
		//TODO
		
		//Graphics Commands
		void SetViewport(RenderBackendViewport* viewports, uint32 numViewports);
		void SetScissors(RenderBackendScissor* scissors, uint32 numScissors);
		void SetStencilReference(uint32 stencilReference);
		void Transitions(RenderBackendBarrier* transitions, uint32 numTransitions);
		void BeginRenderPass(const RenderBackendRenderPassInfo& renderPassInfo);
		void EndRenderPass();
		void Draw(RenderBackendShaderHandle shader, const RenderBackendGraphicsPipelineState,);


	};



}