#include "mikupch.h"
#include "Direct3D12RenderBackend.h"
#include "Direct3D12RenderBackendDefinitions.h"
#include "Direct3D12RenderBackendUtility.h"
#include "Direct3D12RenderBackendPrivate.h"

#include <pix.h>

//过滤调试信息
static constexpr GUID DXGI_DEBUG_ID_ALL = { 0xe48ae283, 0xda80, 0x490b, { 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8 } }; // DXGI_DEBUG_ALL
static constexpr GUID DXGI_DEBUG_ID_DXGI = { 0x25cddaa4, 0xb1c6, 0x47e1, { 0xac, 0x3e, 0x98, 0x87, 0x5b, 0x5a, 0x2e, 0x2a } }; // DXGI_DEBUG_DXGI


//D3D12 调试层调用
static void D3D12MessageCallback(
	D3D12_MESSAGE_CATEGORY Category,// 消息分类
	D3D12_MESSAGE_SEVERITY Severity, // 严重程度）
	D3D12_MESSAGE_ID ID,// 消息ID（具体是哪个问题）
	LPCSTR pDescription, // 人类可读的描述文字
	void* pContext // 用户自定义数据（注册时传入）
) 
{
    switch (Severity)
    {
    case D3D12_MESSAGE_SEVERITY_CORRUPTION:
        MIKU_CORE_FATAL("D3D12 Corruption: ID: {}, Description: {}", (int)ID, pDescription);
        break;
    case D3D12_MESSAGE_SEVERITY_ERROR:
        MIKU_CORE_ERROR("D3D12 Error: ID: {}, Description: {}", (int)ID, pDescription);
        break;
    case D3D12_MESSAGE_SEVERITY_WARNING:
        MIKU_CORE_WARN("D3D12 Warning: ID: {}, Description: {}", (int)ID, pDescription);
        break;
    case D3D12_MESSAGE_SEVERITY_INFO:
        MIKU_CORE_INFO("D3D12 Info: ID: {}, Description: {}", (int)ID, pDescription);
        break;
    case D3D12_MESSAGE_SEVERITY_MESSAGE:
        MIKU_CORE_TRACE("D3D12 Message: ID: {}, Description: {}", (int)ID, pDescription);
        break;
    }
}

namespace MIKU
{
#define COMPILE_RENDER_COMMAND(command,RenderBackendCommandStruct)\
    case RenderBackendCommandStruct::Type:\
        if (!CompileRenderBackendCommand(*reinterpret_cast<const RenderBackendCommandStruct*>(command)))    \
        {                                                                                                   \
            return false;                                                                                   \
        }                                                                                                   \
        break                                                                                                


    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommands(const RenderBackendCommandContainer& container)
    {
        for (uint32 i = 0; i < container.numCommands; i++)
        {
            switch (container.types[i])
            {
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandCopyBuffer);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandCopyTexture);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandUpdateBuffer);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandUpdateTexture);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandClearBufferUAV);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandClearTextureUAV);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBarriers);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBeginTimingQuery);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandEndTimingQuery);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandResolveTimingQueryResults);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatch);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchIndirect);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandSetViewport);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandSetScissor);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandSetStencilReference);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBeginRenderPass);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandEndRenderPass);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDraw);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDrawIndirect);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchMesh);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchMeshIndirect);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBeginDebugLabel);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandEndDebugLabel);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBuildRayTracingBottomLevelAccelerationStructure);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBuildRayTracingTopLevelAccelerationStructure);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchRays);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchSuperSampling);
            default:std::abort(); break;
            }

        }

        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommandsAsynchronous(const RenderBackendCommandContainer& container)
    {
        for (uint32 i = 0; i < container.numCommands; i++)
        {
            switch (container.types[i])
            {
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandClearBufferUAV);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandClearTextureUAV);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBarriers);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBeginTimingQuery);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandEndTimingQuery);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandResolveTimingQueryResults);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatch);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandDispatchIndirect);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBeginDebugLabel);
                COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandEndDebugLabel);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBuildRayTracingBottomLevelAccelerationStructure);
                //COMPILE_RENDER_COMMAND(container.commands[i], RenderBackendCommandBuildRayTracingTopLevelAccelerationStructure);
            default: std::abort(); break;
            }
        }
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandCopyBuffer& command)
    {
        D3D12Buffer* srcBuffer = device->GetBuffer(command.srcBuffer);
        D3D12Buffer* dstBuffer = device->GetBuffer(command.dstBuffer);
        commandList->GetID3D12GraphicsCommandList6()->CopyBufferRegion(dstBuffer->GetID3D12Resource(), command.dstOffset, srcBuffer->GetID3D12Resource(), command.srcOffset, command.bytes);
        return true;

    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandCopyTexture& command)
    {
        D3D12Texture* srcTexture = device->GetTexture(command.srcTexture);
        D3D12Texture* dstTexture = device->GetTexture(command.dstTexture);


        /*
        constexpr UINT D3D12CalcSubresource(
        UINT MipSlice, 哪一层mipmap
        UINT ArraySlice, 数组中的哪一个纹理
        UINT PlaneSlice, 哪个数据平面:比如deepstencil就有两个数据平面
        UINT MipLevels, mipmap层数
        UINT ArraySize 数组大小
        ) noexcept
        */
        D3D12_TEXTURE_COPY_LOCATION dstLocation =
        {
            dstTexture->GetID3D12Resource(),
            D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,//索引方式用子资源定位
            D3D12CalcSubresource(
                command.dstSubresourceLayers.mipLevel,
                command.dstSubresourceLayers.firstLayer,
                0,
                1,
                command.dstSubresourceLayers.arrayLayers)
        };


        D3D12_TEXTURE_COPY_LOCATION srcLocation =
        {
            srcTexture->GetID3D12Resource(),
            D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            D3D12CalcSubresource(
                command.srcSubresourceLayers.mipLevel,
                command.srcSubresourceLayers.firstLayer,
                0,
                1,
                command.srcSubresourceLayers.arrayLayers)
        };

        D3D12_BOX srcBox =
        {
            (UINT)command.srcOffset.x,
			(UINT)command.srcOffset.y,
			(UINT)command.srcOffset.z,
			(UINT)command.srcOffset.x + command.extent.width,
			(UINT)command.srcOffset.y + command.extent.height,
			(UINT)command.srcOffset.z + command.extent.depth
        };

        commandList->GetID3D12GraphicsCommandList6()->CopyTextureRegion(&dstLocation,command.dstOffset.x,command.dstOffset.y,command.dstOffset.z, &srcLocation, &srcBox);

        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandUpdateBuffer& command)
    {
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandUpdateTexture& command)
    {
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandClearBufferUAV& command) 
    {
        D3D12Buffer* buffer = device->GetBuffer(command.buffer);

        const UINT clearValue[4] =
        {
            command.data,
            command.data,
            command.data,
            command.data
        };
    
        D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle = device->resourceDescriptorHeap->gpuDescriptorHandle;
        viewGPUHandle.ptr += buffer->bindlessResourceDescriptorIndexUAV * device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle = buffer->descriptor;

        commandList->GetID3D12GraphicsCommandList()->ClearUnorderedAccessViewUint(viewGPUHandle, viewCPUHandle, buffer->GetID3D12Resource(), clearValue, 0, nullptr);

        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandClearTextureUAV& command)
    {
        D3D12Texture* texture = device->GetTexture(command.uav.texture);

        const UINT clearValue[4] =
        {
        	command.clearValue.colorValue.uint32[0],
            command.clearValue.colorValue.uint32[1],
			command.clearValue.colorValue.uint32[2],
			command.clearValue.colorValue.uint32[3] 
		};
        
        if (!command.clearValue.test) 
        {
            D3D12UnorderedAccessView* uav = texture->GetUnorderedAccessView(command.uav.mipLevel);
            D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandle = device->resourceDescriptorHeap->gpuDescriptorHandle;
            viewGPUHandle.ptr += uav->bindlessIndex * device->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle = uav->descriptor;
            
            commandList->GetID3D12GraphicsCommandList()->ClearUnorderedAccessViewUint(viewGPUHandle, viewCPUHandle, texture->GetID3D12Resource(), clearValue, 0, nullptr);

        }
        else 
        {
            const FLOAT clearRGBA[4] =
            {
                command.clearValue.colorValue.float32[0],
                command.clearValue.colorValue.float32[1],
                command.clearValue.colorValue.float32[2],
                command.clearValue.colorValue.float32[3]
            };

            D3D12RenderTargetView* rtv = texture->GetRenderTargetView(0);
            D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle = rtv->descriptor;
            D3D12_RECT rect =
            {
                0,
                0,
                LONG(texture->width),
                LONG(texture->height)
            };
            commandList->GetID3D12GraphicsCommandList()->ClearRenderTargetView(viewCPUHandle, clearRGBA, 1, &rect);
        }
        return true;
    
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBarriers& command) 
    {

        std::vector<D3D12_GLOBAL_BARRIER> globalBarriers;
        std::vector<D3D12_TEXTURE_BARRIER> textureBarriers;
        std::vector<D3D12_BUFFER_BARRIER> bufferBarriers;

        for (uint32 i = 0; i < command.barrierCount; i++) 
        {
            const RenderBackendBarrier& barrier = command.barriers[i];
            
            switch (barrier.type) 
            {
            case RenderBackendBarrier::Type::Global: 
            {

            }break;
            case RenderBackendBarrier::Type::Texture: 
            {
                D3D12Texture* texture = device->GetTexture(barrier.texture);
                D3D12_BARRIER_SUBRESOURCE_RANGE subresourceRange = {};
                subresourceRange.IndexOrFirstMipLevel = barrier.textureRange.firstLevel;
                subresourceRange.NumMipLevels = (barrier.textureRange.mipLevels == RenderBackendTextureSubresourceRange::RemainingMipLevels) ? (texture->mipLevels - barrier.textureRange.firstLevel) : barrier.textureRange.mipLevels;
                subresourceRange.FirstArraySlice = barrier.textureRange.firstLayer;
                subresourceRange.NumArraySlices = (barrier.textureRange.arrayLayers == RenderBackendTextureSubresourceRange::RemainingArrayLayers) ? (texture->arraySize - barrier.textureRange.firstLayer) : barrier.textureRange.arrayLayers;
                subresourceRange.FirstPlane = 0;
                subresourceRange.NumPlanes = 1; // @todo

                D3D12_BARRIER_SYNC syncBefore = D3D12_BARRIER_SYNC_ALL; //屏障之前需要等待完成的 GPU 工作阶段
                D3D12_BARRIER_SYNC syncAfter = D3D12_BARRIER_SYNC_ALL; //屏障之后需要阻塞直到屏障完成的 GPU 工作阶段
                D3D12_BARRIER_ACCESS accessBefore = D3D12_BARRIER_ACCESS_COMMON;
                D3D12_BARRIER_ACCESS accessAfter = D3D12_BARRIER_ACCESS_COMMON;
                D3D12_BARRIER_LAYOUT layoutBefore = D3D12_BARRIER_LAYOUT_UNDEFINED;
                D3D12_BARRIER_LAYOUT layoutAfter = D3D12_BARRIER_LAYOUT_UNDEFINED;
                D3D12_TEXTURE_BARRIER_FLAGS flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

                ConvertToD3D12TextureBarrier(
                    barrier.stateBefore,
                    barrier.stateAfter,
                    syncBefore,
                    syncAfter,
                    accessBefore,
                    accessAfter,
                    layoutBefore,
                    layoutAfter,
                    flags
                );

                D3D12_TEXTURE_BARRIER& textureBarrier = textureBarriers.emplace_back();
                textureBarrier.SyncBefore = syncBefore;
                textureBarrier.SyncAfter = syncAfter;
                textureBarrier.AccessBefore = accessBefore;
                textureBarrier.AccessAfter = accessAfter;
                textureBarrier.LayoutBefore = layoutBefore;
                textureBarrier.LayoutAfter = layoutAfter;
                textureBarrier.pResource = texture->GetID3D12Resource();
                textureBarrier.Subresources = subresourceRange;
                textureBarrier.Flags = flags;
            }break;
            case RenderBackendBarrier::Type::Buffer:
            {
                D3D12Buffer* buffer = device->GetBuffer(barrier.buffer);

                D3D12_BARRIER_SYNC syncBefore = D3D12_BARRIER_SYNC_ALL;
                D3D12_BARRIER_SYNC syncAfter = D3D12_BARRIER_SYNC_ALL;
                D3D12_BARRIER_ACCESS accessBefore = D3D12_BARRIER_ACCESS_COMMON;
                D3D12_BARRIER_ACCESS accessAfter = D3D12_BARRIER_ACCESS_COMMON;

                ConvertToD3D12BufferBarrier(
                    barrier.stateBefore,
                    barrier.stateAfter,
                    syncBefore,
                    syncAfter,
                    accessBefore,
                    accessAfter);
                D3D12_BUFFER_BARRIER& bufferBarrier = bufferBarriers.emplace_back();
                bufferBarrier.SyncBefore = syncBefore;
                bufferBarrier.SyncAfter = syncAfter;
                bufferBarrier.AccessBefore = accessBefore;
                bufferBarrier.AccessAfter = accessAfter;
                bufferBarrier.pResource = buffer->GetID3D12Resource();
                bufferBarrier.Offset = barrier.bufferRange.offset;
                bufferBarrier.Size = barrier.bufferRange.size;

            }break;
            default: std::abort(); break;
                
            }
        }

        std::vector<D3D12_BARRIER_GROUP> barrierGroups;
        if (!globalBarriers.empty()) 
        {
            D3D12_BARRIER_GROUP& group = barrierGroups.emplace_back();
            group.Type = D3D12_BARRIER_TYPE_GLOBAL;
            group.NumBarriers = (UINT32)globalBarriers.size();
            group.pGlobalBarriers = globalBarriers.data();
        }
        if (!textureBarriers.empty()) 
        {
            D3D12_BARRIER_GROUP& group = barrierGroups.emplace_back();
            group.Type = D3D12_BARRIER_TYPE_TEXTURE;
            group.NumBarriers = (UINT32)textureBarriers.size();
            group.pTextureBarriers = textureBarriers.data();
        }
        if (!bufferBarriers.empty())
        {
            D3D12_BARRIER_GROUP& group = barrierGroups.emplace_back();
            group.Type = D3D12_BARRIER_TYPE_BUFFER;
            group.NumBarriers = (UINT32)bufferBarriers.size();
            group.pBufferBarriers = bufferBarriers.data();
        }

        if (!barrierGroups.empty()) 
        {
            commandList->GetID3D12GraphicsCommandList7()->Barrier((UINT)barrierGroups.size(),barrierGroups.data());
        }

		return true;
        
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBeginTimingQuery& command) 
    {
        const auto& timingQueryHeap = device->GetTimingQueryHeap(command.timingQueryHeap);
        
        uint32 queryIndex = command.region * 2 + 0;
        assert(queryIndex < timingQueryHeap->maxQueryCount);
        
        commandList->GetID3D12GraphicsCommandList6()->EndQuery(timingQueryHeap->GetID3D12QueryHeap(), D3D12_QUERY_TYPE_TIMESTAMP, queryIndex);
       
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandEndTimingQuery& command)
    {
        const auto& timingQueryHeap = device->GetTimingQueryHeap(command.timingQueryHeap);

        uint32 queryIndex = command.region * 2 + 1;
        assert(queryIndex < timingQueryHeap->maxQueryCount);

        commandList->GetID3D12GraphicsCommandList6()->EndQuery(timingQueryHeap->GetID3D12QueryHeap(), D3D12_QUERY_TYPE_TIMESTAMP, queryIndex);
        
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandResolveTimingQueryResults& command) 
    {
    
    
    }

}