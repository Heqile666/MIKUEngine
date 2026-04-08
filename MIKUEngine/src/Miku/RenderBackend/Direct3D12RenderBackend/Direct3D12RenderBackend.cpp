#include "mikupch.h"
#include "Direct3D12RenderBackend.h"
#include "Direct3D12RenderBackendDefinitions.h"
#include "Direct3D12RenderBackendUtility.h"
#include "Direct3D12RenderBackendPrivate.h"

#include <pix.h>

#pragma comment(lib, "d3d12.lib")//链接库

//
static constexpr GUID DXGI_DEBUG_ID_ALL = { 0xe48ae283, 0xda80, 0x490b, { 0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8 } }; // DXGI_DEBUG_ALL

//这段代码定义了一个编译期常量 GUID，用于标识 DXGI 调试系统中的 DXGI 组件。
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


    //通过两次EndQuery来记录时间,做差值来测GPU时间
    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBeginTimingQuery& command) 
    {
        const auto& timingQueryHeap = device->GetTimingQueryHeap(command.timingQueryHeap);
        
        uint32 queryIndex = command.region * 2 + 0;//偶数槽位
        assert(queryIndex < timingQueryHeap->maxQueryCount);
        
        commandList->GetID3D12GraphicsCommandList6()->EndQuery(timingQueryHeap->GetID3D12QueryHeap(), D3D12_QUERY_TYPE_TIMESTAMP, queryIndex);
       
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandEndTimingQuery& command)
    {
        const auto& timingQueryHeap = device->GetTimingQueryHeap(command.timingQueryHeap);

        uint32 queryIndex = command.region * 2 + 1;//奇数槽位
        assert(queryIndex < timingQueryHeap->maxQueryCount);

        commandList->GetID3D12GraphicsCommandList6()->EndQuery(timingQueryHeap->GetID3D12QueryHeap(), D3D12_QUERY_TYPE_TIMESTAMP, queryIndex);
        
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandResolveTimingQueryResults& command) 
    {
        //todo
        // OPTICK_EVENT();
        const auto& timingQueryHeap = device->GetTimingQueryHeap(command.timingQueryHeap);
        //复制查询结果到缓冲区
        const auto& buffer = device->GetBuffer(command.buffer);

        //一个region代表两个查询时间戳,起始查询时间戳和结束查询时间戳,region是引擎上层的概念
        //底层dx12需要的是时间戳
        //regionStart代表起始时间戳的索引
        //regionCount代表查询时间戳的个数
        //这里乘*2是将时间戳转成region
        uint32 queryStart = 2 * command.regionStart;
        uint32 queryCount = 2 * command.regionCount;
        
        commandList->GetID3D12GraphicsCommandList6()->ResolveQueryData(
            timingQueryHeap->GetID3D12QueryHeap(),
            D3D12_QUERY_TYPE_TIMESTAMP,
            queryStart,
			queryCount,
			buffer->GetID3D12Resource(),
			command.offset
        );
        return true;
    }

    bool D3D12RenderBackendCommandListContext::PrepareForDispatch(RenderBackendShaderHandle computerShader, const RenderBackendPushConstantValues& pushConstantValues) 
    {
        D3D12Shader* d3d12Shader = device->GetShader(computerShader);
        D3D12ComputePipelineState* pipelineState = device->FindOrCreateComputePipelineState(d3d12Shader);
        if (pipelineState->GetID3D12PipelineState() != activeComputePipeline) 
        {
            ID3D12RootSignature* rootSignature = device->GetID3D12RootSignature();
            
            commandList->GetID3D12GraphicsCommandList6()->SetPipelineState(pipelineState->GetID3D12PipelineState());

            activeComputePipeline = pipelineState->GetID3D12PipelineState();
        }
    
        if (RenderBackendPushConstantsBytes > 0) 
        {
            const void* pushConstantData = &pushConstantValues.data;
            commandList->GetID3D12GraphicsCommandList6()->SetComputeRoot32BitConstants(
                0,//根签名的第几个参数
                RenderBackendPushConstantsBytes / 4,//写入几个32位值
                pushConstantData,//传入的数据
                0
            );
        
        }
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDispatch& command) 
    {
        //todo
        //OPTICK_EVENT();
        if (!PrepareForDispatch(command.computeShader, command.pushConstantValues)) 
        {
            return false;
        }
        commandList->GetID3D12GraphicsCommandList6()->Dispatch(command.threadGroupCountX, command.threadGroupCountY, command.threadGroupCountZ);
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDispatchIndirect& command)
    {
        //OPTICK_EVENT();
        if (!PrepareForDispatch(command.computeShader, command.pushConstantValues))
        {
            return false;
        }
        commandList->GetID3D12GraphicsCommandList6()->ExecuteIndirect(
            device->GetDispatchIndirectCommandSignature(),//命令签名,用于描述pArgumentBuffer中各个参数的具体含义
            1,//最大命令数
            device->GetBuffer(command.argumentBuffer)->GetID3D12Resource(),//参数pArgumentBuffer,上一个cs写入的数据到pArgumentBuffer
            command.argumentBufferOffset,//参数pArgumentBuffer偏移
            nullptr,//计数buffer，动态控制实际执行的命令数，如果不填，执行参数2指定的命令数
            0//和参数5配合使用，参数5为nullptr时，该参数无效
        );
        return true; 
    }

    //bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBuildRayTracingBottomLevelAccelerationStructure& command)

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandSetViewport& command) 
    {
        D3D12_VIEWPORT viewports[RenderBackendMaxViewportCount];
        for (uint32 i = 0; i < command.viewportCount; i++) 
        {
            viewports[i] = 
            {
                command.viewports[i].x,
				command.viewports[i].y,
				command.viewports[i].width,
				command.viewports[i].height,
				command.viewports[i].minDepth,
				command.viewports[i].maxDepth
            };
        
        }
       
        commandList->GetID3D12GraphicsCommandList6()->RSSetViewports(command.viewportCount, viewports);
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandSetScissor& command)
    {
        D3D12_RECT scissors[RenderBackendMaxViewportCount];
        for (uint32 i = 0; i < command.scissorCount; i++)
        {
            scissors[i] =
            {
                command.scissors[i].left,
                command.scissors[i].top,
                command.scissors[i].left + (int32)command.scissors[i].width,
                command.scissors[i].top + (int32)command.scissors[i].height
            };
        }

        commandList->GetID3D12GraphicsCommandList6()->RSSetScissorRects(command.scissorCount, scissors);
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandSetStencilReference& command)
    {
        commandList->GetID3D12GraphicsCommandList6()->OMSetStencilRef(command.stencilReference);
        return true;
    }


#define EXPORT_D3D12_REDIST_CONSTANT_DATA (_WIN64 && 1)

#if EXPORT_D3D12_REDIST_CONSTANT_DATA
    //导出参数告诉windows使用项目自带的d3d12
    // D3D12SDKVersion declares the SDK version of the D3D12 redistributable that the Application is targeting.
    extern "C" {_declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }

    //todo
    /**
    C++ 规则：
    const int x = 10;          // 内部链接（只在本文件可见）
    int y = 20;                // 外部链接（其他文件可见）

    extern 覆盖 const 的默认行为：
    extern const int x = 10;   // 外部链接（其他文件也能看到）
    **/
    // D3D12SDKPath is a UTF-8 string that declares that D3D12Core.dll, D3D12SDKLayers.dll, and other D3D12 redist binaries are located in the subfolder D3D12 relative to the exe.
    extern "C" {_declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }
#endif

    bool D3D12RenderBackend::Init(const RenderBackendDesc* desc)
    {
        if (false)
        {
            //todo
#if !MIKU_CONFIGURATION_RELEASE
            const GUID experimentalFeatures[] =
            {
                D3D12ExperimentalShaderModels

            };

            if (SUCCEEDED(D3D12EnableExperimentalFeatures(_countof(experimentalFeatures), experimentalFeatures, nullptr, nullptr)))
            {
                MIKU_CORE_INFO("Agility SDK not foud");

            }

#else
            MIKU_CORE_WARN("Try to enable experimental features in the release build, ignored.");
#endif
         
        }
        DWORD dxgiFactoryFlags = 0;

        useDebugLayers = desc->enableDebugLayer;
        useGPUBasedValidation = desc->enableDebugLayer;

       /* for (uint32 i = 0; i < desc->featureCount; i++)
        {
            if (desc->features[i] == RenderBackendFeature::HardwareRayTracing)
            {
                enableHardwareRayTracing = true;
            }
        }*/


        if (useDebugLayers) 
        {
            Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)))) 
            {
                debugInterface->EnableDebugLayer();
                MIKU_CORE_INFO("D3D12 debug validation layer enabled");
                if (useGPUBasedValidation) 
                {
                    Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
                    if (SUCCEEDED(debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)))) 
                    {
                        debugInterface1->SetEnableGPUBasedValidation(true);
                        MIKU_CORE_INFO("D3D12 GPU based debug validation layer enabled");
                    
                    }
                    else 
                    {
                        MIKU_CORE_WARN("Unable to enable D3D12 GPU based debug validation layer");
                        
                    }

                }


                /*
                    获取 DXGI 调试消息
                    读取/过滤 debug message queue
                */
                Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
                if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) 
                {
                    /*
                        DXGI_CREATE_FACTORY_DEBUG是获取
                        swap chain 相关问题
                        adapter/factory 相关问题
                        DXGI 层错误信息
                    */
                    dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
                    
                    /*
                        告诉 DXGI：
                        如果出现 Corruption
                        或 Error
                        或 Warning
                        那么在调试器下就触发中断。
                        DXGI_DEBUG_ID_ALL
                        表示对所有 DXGI 调试生产者生效。
                        也就是不只针对某一个模块，而是所有 DXGI 相关消息源。
                    */
                    dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ID_ALL,DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,true);
                    dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ID_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
                    dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ID_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING, true);
                

                    //需要屏蔽的DXGI消息ID
                    DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
                    {
                        80 /* IDXGISwapChain::GetContainingOutput: The swap chain's adapter does not control the output on which the swap chain's window resides.
                            这个 swap chain 所属的适配器，并不控制该 swap chain 窗口当前所在的输出设备（output/显示器）。
                           */,
                    };

                    /*
                    给 DXGI producer 配一个过滤规则，

                    把某些指定 ID 的消息加入 拒绝列表（DenyList），

                    从而让这些消息不要被存进 DXGI Info Queue。
                    */
                    DXGI_INFO_QUEUE_FILTER filter = {};
                    filter.DenyList.NumIDs =  _countof(hide);
                    filter.DenyList.pIDList = hide;

                    dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_ID_DXGI, &filter);
                }



            
            }
            else 
            {
                MIKU_CORE_WARN("Unable to enable D3D12 debug validation layer");

            }

        }

        dxgiLibraryHandle = LoadLibraryW(L"dxgi.dll");

        /*
            int add(int a,int b);正常函数声明
            int (*pfnAdd)(int,int)函数指针
            pfnAdd = &add;

            typedef int INT,类型是 int ,别名是 INT
            typedef int(* pfn_ADD)(int,int) ,类型是int(*)(int,int),别名是pfn_ADD

            WINAPI:
            它指定这个函数使用 Windows API 的调用约定。
            主要意义：
            参数怎么传
            谁负责清理栈
            二进制接口怎么兼容

            _COM_Outptr_ 不是 C++ 语法关键字，
            它是一个 SAL 注解（Source Annotation Language），也就是“源代码注解”。

            它主要给：
            编译器静态分析,做一些检查，如_COM_Outptr_ void 传入空指针告警等
            IDE 提示
            代码阅读者
            看用的。
            _COM_Outptr_ 只是告诉工具和程序员：
            这是一个 COM 风格的输出参数，函数会通过它返回一个接口指针。
        */
        typedef HRESULT(WINAPI* PFN_CreateDXGIFactory2)(UINT, REFIID, _COM_Outptr_ void**);

        /*
            在已经加载的 DLL 中
            按函数名字查找导出函数
            返回这个函数的地址
        */
        PFN_CreateDXGIFactory2 pfnCreateDXGIFactory2 = reinterpret_cast<PFN_CreateDXGIFactory2>(reinterpret_cast<void*>(GetProcAddress(dxgiLibraryHandle, "CreateDXGIFactory2")));
        assert(pfnCreateDXGIFactory2);

        HRESULT hr = pfnCreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
        if (FAILED(hr)) 
        {
            MIKU_CORE_WARN("Error: CreateDXGIFactory2 failed!");
            return false; 
        }

        Microsoft::WRL::ComPtr<IDXGIFactory5> dxgiFactory5;
        if (SUCCEEDED(dxgiFactory5->QueryInterface(IID_PPV_ARGS(&dxgiFactory5)))) 
        {
            BOOL allowTearing = FALSE;
            /*
            DXGI_FEATURE_PRESENT_ALLOW_TEARING是否可以支持无垂直同步
            */
            if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing)))) 
            {
                tearingSupported = (allowTearing != FALSE)? true:false;
            }
        
        }
        Microsoft::WRL::ComPtr<IDXGIAdapter1> currentAdapter;
        for (uint32 adpeterIndex = 0; dxgiFactory5->EnumAdapters1(adpeterIndex, &currentAdapter) != DXGI_ERROR_NOT_FOUND ;adpeterIndex++) 
        {   
            if (currentAdapter) 
            {
                {
                    DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
                    //获取物理显卡的相关信息
                    D3D12_CHECK(currentAdapter->GetDesc1(&dxgiAdapterDesc));
                    
                    MIKU_CORE_INFO("Found D3D12 Adapter {}: {}", adpeterIndex,dxgiAdapterDesc.Description);
                
                    MIKU_CORE_INFO("Adapter has {} MB of dedicated video memory, {}MB of dedicated system memory, and {}MB of shared system memory",
                        (uint32)(dxgiAdapterDesc.DedicatedVideoMemory / (1024 * 1024)),
                        (uint32)(dxgiAdapterDesc.DedicatedSystemMemory / (1024 * 1024)),
                        (uint32)(dxgiAdapterDesc.SharedSystemMemory / (1024 * 1024)));

                    D3D12Adapter* newAdapter = new D3D12Adapter();
                    newAdapter->adapterIndex = adpeterIndex;
                    newAdapter->desc = dxgiAdapterDesc;
                    newAdapter->dxgiAdapter = currentAdapter;

                    adapters.push_back(newAdapter);
                    numAdapters++;

                }

            }
        
        }
        return true;
    }

    void D3D12RenderBackend::Exit() 
    {
        DestroyRenderDevices();

        {
            typedef HRESULT(WINAPI* PFN_DXGIGetDebugInterface1)(UINT, REFIID, _COM_Outptr_ void**);
            PFN_DXGIGetDebugInterface1 pfnDXGIGetDebugInterface1 = reinterpret_cast<PFN_DXGIGetDebugInterface1>(reinterpret_cast<void*>(GetProcAddress(dxgiLibraryHandle, "DXGIGetDebugInterface1")));
            if (pfnDXGIGetDebugInterface1 != nullptr) 
            {
                /*
                    作用一般是做 DXGI 调试相关操作，比如：
                    报告存活对象
                    调试运行时状态查看
                */
                Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebugInterface;
                if (SUCCEEDED(pfnDXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugInterface)))) 
                {   /*
                    DXGI_DEBUG_RLO_SUMMARY
                    只输出摘要信息（summary）
                    DXGI_DEBUG_RLO_IGNORE_INTERNAL
                    忽略 DXGI / D3D 内部自己持有的内部对象
                    */
                    
                    dxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_ID_ALL, static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
                
                }
            }
        }
        FreeLibrary(dxgiLibraryHandle);
    
    }

    void D3D12RenderBackend::Tick() 
    {
        for (uint32 deviceIndex = 0; deviceIndex < numDevices; deviceIndex++) 
        {
            D3D12Device* device = devices[deviceIndex];
            device->Tick();
        }
    }

    void D3D12RenderBackend::CreateRenderDevices(PhysicalDeviceID* physicalDeviceIDs, uint32 numDevices, uint32* outDeviceMask) 
    {
        {
            D3D12Adapter* adapter = adapters[0];
            devices[0] = new D3D12Device(this);
            D3D12Device* device = devices[0];
            d3d12Devices[0] = device->device;
            if (device->Init(this,adapter)) 
            {
            
            }
            this->numDevices++;
        }
    }

    void D3D12RenderBackend::DestroyRenderDevices() 
    {
        D3D12Device* device = devices[0];

        device->Exit();
        delete device;

        Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device = d3d12Devices[0];

        if (d3d12Device && useDebugLayers)
        {
            Microsoft::WRL::ComPtr<ID3D12DebugDevice> debugDevice;
            if (SUCCEEDED(device->device->QueryInterface(IID_PPV_ARGS(&debugDevice))))
            {
                D3D12_RLDO_FLAGS rldoFlags = D3D12_RLDO_DETAIL;
                D3D12_CHECK(debugDevice->ReportLiveDeviceObjects(rldoFlags))
            }
        }
    }

    void D3D12RenderBackend::FlushRenderDevices() 
    {
        D3D12Device* device = devices[0];
        device->WaitIdle();
    }

    RenderBackendDeviceContext D3D12RenderBackend::GetNativeDevice() 
    {
        RenderBackendDeviceContext d = {};
        d.device = devices[0]->device.Get();
        d.physicalDevice = nullptr;
        return d;
    }

    uint32 D3D12Device::CreateD3D12SwapChain(const RenderBackendSwapChainDesc* desc) 
    {
        D3D12SwapChain* swapChain = new D3D12SwapChain();

        ID3D12CommandQueue* commandQueue = GetCommandQueue(D3D12CommandQueueType::Direct)->GetID3D12CommandQueue();

        HWND windowHandle = (HWND)desc->windowHandle;

        UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        if(backend->)
    }
}