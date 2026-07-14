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

        commandList->GetID3D12GraphicsCommandList6()->CopyTextureRegion(&dstLocation, command.dstOffset.x, command.dstOffset.y, command.dstOffset.z, &srcLocation, &srcBox);

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
            commandList->GetID3D12GraphicsCommandList7()->Barrier((UINT)barrierGroups.size(), barrierGroups.data());
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


    //D3D12_RENDER_BACKEND_USE_RENDER_PASS告诉DX12接下来的一组Draw 调用都画到哪些RenderTarget 上，开始前怎样处理旧数据，结束后如何保存结果
    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBeginRenderPass& command)
    {
#if D3D12_RENDER_BACKEND_USE_RENDER_PASS

        // TODO: optimize this, to avoid recreate PSOs.
        memset(&activeRenderPass, 0, sizeof(activeRenderPass));

        activeRenderPass.numRenderTargets = 0;
        activeRenderPass.hasDepthStencil = false;

        UINT numRenderTargets = 0;
        bool hasDepthStencil = false;

        D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargetDescs[RenderBackendMaxRenderTargetCount] = {};
        D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc = {};

        for (uint32 index = 0; index < RenderBackendMaxRenderTargetCount; index++)
        {
            const RenderBackendRenderTargetBinding& renderTarget = command.renderPassInfo.renderTargets[index];

            // TODO: remove this
            if (!renderTarget.texture)
            {
                continue;
            }

            D3D12Texture* texture = device->GetTexture(renderTarget.texture);

            D3D12_RENDER_PASS_RENDER_TARGET_DESC& renderTargetDesc = renderTargetDescs[numRenderTargets];
            renderTargetDesc.cpuDescriptor = texture->GetRenderTargetView(renderTarget.mipLevel)->descriptor;
            renderTargetDesc.BeginningAccess.Type = ConvertToD3D12RenderPassBeginningAccessType(renderTarget.loadOperation);
            renderTargetDesc.BeginningAccess.Clear.ClearValue = texture->clearValue;
            renderTargetDesc.EndingAccess.Type = ConvertToD3D12RenderPassEndingAccessType(renderTarget.storeOperation);

            numRenderTargets++;
            activeRenderPass.numRenderTargets = numRenderTargets;
            activeRenderPass.renderTargetFormats[numRenderTargets - 1] = texture->format;
        }

        if (command.renderPassInfo.depthStencil.texture)
        {
            const RenderBackendDepthStencilBinding& depthStencil = command.renderPassInfo.depthStencil;

            D3D12Texture* texture = device->GetTexture(depthStencil.texture);

            depthStencilDesc.cpuDescriptor = texture->GetDepthStencilView(GetDepthStencilViewIndex(depthStencil.depthStencilAccessType))->descriptor;
            depthStencilDesc.DepthBeginningAccess.Type = ConvertToD3D12RenderPassBeginningAccessType(depthStencil.depthLoadOperation);
            depthStencilDesc.DepthBeginningAccess.Clear.ClearValue = texture->clearValue;
            depthStencilDesc.DepthEndingAccess.Type = ConvertToD3D12RenderPassEndingAccessType(depthStencil.depthStoreOperation);
            depthStencilDesc.StencilBeginningAccess.Type = ConvertToD3D12RenderPassBeginningAccessType(depthStencil.stencilLoadOperation);
            depthStencilDesc.StencilEndingAccess.Type = ConvertToD3D12RenderPassEndingAccessType(depthStencil.stencilStoreOperation);

            hasDepthStencil = true;
            activeRenderPass.hasDepthStencil = true;
            activeRenderPass.depthStencilViewFormat = texture->format;
        }

        D3D12_RENDER_PASS_FLAGS flags = D3D12_RENDER_PASS_FLAG_NONE;
        if (command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthReadOnly_StencilNoAccess ||
            command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthReadOnly_StencilWrite ||
            command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthReadOnly_StencilReadOnly)
        {
            flags |= D3D12_RENDER_PASS_FLAG_BIND_READ_ONLY_DEPTH;
        }
        if (command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthNoAccess_StencilReadOnly ||
            command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthWrite_StencilReadOnly ||
            command.renderPassInfo.depthStencil.depthStencilAccessType == RenderBackendDepthStencilAccessType::DepthReadOnly_StencilReadOnly)
        {
            flags |= D3D12_RENDER_PASS_FLAG_BIND_READ_ONLY_STENCIL;
        }

        commandList->GetID3D12GraphicsCommandList6()->BeginRenderPass(numRenderTargets, renderTargetDescs, hasDepthStencil ? &depthStencilDesc : nullptr, flags);
#else
        activeRenderPass.numRenderTargets = 0;
        activeRenderPass.hasDepthStencil = false;

        UINT numRenderTargets = 0;
        bool hasDepthStencil = false;

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViews[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = {};

        for (uint32 index = 0; index < RenderBackendMaxRenderTargetCount; index++)
        {
            const auto& renderTarget = command.renderPassInfo.renderTargets[index];
            // TODO: remove this
            if (!renderTarget.texture)
            {
                continue;
            }

            D3D12Texture* texture = device->GetTexture(renderTarget.texture);

            renderTargetViews[index] = texture->GetRenderTargetView(renderTarget.mipLevel)->descriptor;

            numRenderTargets++;
            activeRenderPass.numRenderTargets = numRenderTargets;
            activeRenderPass.renderTargetFormats[numRenderTargets - 1] = texture->format;

            if (renderTarget.loadOp == RenderBackendRenderPassLoadOperation::Clear)
            {
                D3D12_RECT clearRect = {};
                clearRect.left = 0;
                clearRect.top = 0;
                clearRect.right = texture->width - 1;
                clearRect.bottom = texture->height - 1;

                commandList->GetID3D12GraphicsCommandList6()->ClearRenderTargetView(renderTargetViews[index], texture->clearValue.Color, 1, &clearRect);
            }
        }

        if (command.renderPassInfo.depthStencil.texture)
        {
            const auto& depthStencil = command.renderPassInfo.depthStencil;

            D3D12Texture* texture = device->GetTexture(depthStencil.texture);

            depthStencilView = texture->GetDepthStencilView(GetDepthStencilViewIndex(depthStencil.depthStencilAccessType))->descriptor;

            hasDepthStencil = true;
            activeRenderPass.hasDepthStencil = true;
            activeRenderPass.depthStencilViewFormat = texture->format;

            D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAGS(0);

            if (depthStencil.depthLoadOp == RenderBackendRenderPassLoadOperation::Clear)
            {
                clearFlags |= D3D12_CLEAR_FLAG_DEPTH;
            }

            if (depthStencil.stencilLoadOp == RenderBackendRenderPassLoadOperation::Clear)
            {
                clearFlags |= D3D12_CLEAR_FLAG_STENCIL;
            }

            if (clearFlags != 0)
            {
                D3D12_RECT clearRect = {};
                clearRect.left = 0;
                clearRect.top = 0;
                clearRect.right = texture->width - 1;
                clearRect.bottom = texture->height - 1;

                commandList->GetID3D12GraphicsCommandList6()->ClearDepthStencilView(depthStencilView, clearFlags, texture->clearValue.DepthStencil.Depth, texture->clearValue.DepthStencil.Stencil, 1, &clearRect);
            }
        }

        commandList->GetID3D12GraphicsCommandList6()->OMSetRenderTargets(numRenderTargets, renderTargetViews, FALSE, hasDepthStencil ? &depthStencilView : nullptr);
#endif

        insideRenderPass = true;
        return true;

    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandEndRenderPass& command)
    {
#if D3D12_RENDER_BACKEND_USE_RENDER_PASS
        commandList->GetID3D12GraphicsCommandList6()->EndRenderPass();
#endif
        insideRenderPass = false;
        return true;
    }

    bool D3D12RenderBackendCommandListContext::PrepareForDraw(RenderBackendShaderHandle vertexShader, RenderBackendShaderHandle pixelShader, const RenderBackendGraphicsPipelineStateDescription& pipelineStateDesc, RenderBackendPrimitiveTopology topology, RenderBackendBufferHandle indexBuffer, const RenderBackendPushConstantValues& pushConstantValues)
    {
        assert(insideRenderPass);
        D3D12GraphicsPipelineState* pipelineState = device->FindOrCreateGraphicsPipelineState(device->GetShader(vertexShader), device->GetShader(pixelShader), nullptr, nullptr, pipelineStateDesc, &activeRenderPass, topology);
        if (pipelineState->GetID3D12PipelineState() != activeGraphicsPipeline)
        {
            ID3D12RootSignature* rootSignature = device->rootSignature.Get();
            commandList->GetID3D12GraphicsCommandList6()->SetPipelineState(pipelineState->GetID3D12PipelineState());

            D3D_PRIMITIVE_TOPOLOGY primitiveTopology = ConvertToD3D12PrimitiveTopology(topology);
            commandList->GetID3D12GraphicsCommandList6()->IASetPrimitiveTopology(primitiveTopology);

            activeGraphicsPipeline = pipelineState->GetID3D12PipelineState();
        }

        if (indexBuffer)
        {
            const D3D12Buffer* buffer = device->GetBuffer(indexBuffer);
            D3D12_INDEX_BUFFER_VIEW indexBufferView =
            {
                buffer->gpuAddress,
                (UINT)buffer->size,
                DXGI_FORMAT_R32_UINT
            };
            commandList->GetID3D12GraphicsCommandList6()->IASetIndexBuffer(&indexBufferView);
        }

        if (RenderBackendPushConstantsBytes > 0)
        {
            const void* pushConstantsData = &pushConstantValues.data;
            commandList->GetID3D12GraphicsCommandList6()->SetGraphicsRoot32BitConstants(
                0,
                RenderBackendPushConstantsBytes / 4,
                pushConstantsData,
                0
            );
        }

        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDraw& command)
    {
        //todo
        //OPTICK_EVENT();

        if (!PrepareForDraw(command.vertexShader, command.pixelShader, command.pipelineState, command.topology, command.indexBuffer, command.pushConstantValues))
        {
            return false;
        }
        if (!command.indexBuffer)
        {
            commandList->GetID3D12GraphicsCommandList6()->DrawInstanced(
                command.draw.vertexCount,
                command.draw.instanceCount,
                command.draw.firstVertex,
                command.draw.firstInstance
            );
        }
        else
        {
            commandList->GetID3D12GraphicsCommandList6()->DrawIndexedInstanced(
                command.drawIndexed.indexCount,
                command.drawIndexed.instanceCount,
                command.drawIndexed.firstIndex,
                command.drawIndexed.vertexOffset,
                command.drawIndexed.firstInstance
            );
        }
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDrawIndirect& command)
    {
        if (!PrepareForDraw(command.vertexShader, command.pixelShader, command.pipelineState, command.topology, command.indexBuffer, command.pushConstantValues))
        {
            return false;
        }

        if (!command.indexBuffer)
        {
            commandList->GetID3D12GraphicsCommandList6()->ExecuteIndirect(
                device->GetDrawIndirectCommandSignature(),
                command.drawCount,
                device->GetBuffer(command.argumentBuffer)->GetID3D12Resource(),
                command.argumentBufferOffset,
                nullptr,//执行此命令的数量，与command.drawCount比较取最小值
                0//在CountBuffer中的偏移量
            );
        }
        else
        {
            commandList->GetID3D12GraphicsCommandList6()->ExecuteIndirect(
                device->GetDrawIndexedIndirectCommandSignature(),
                command.drawCount,
                device->GetBuffer(command.argumentBuffer)->GetID3D12Resource(),
                command.argumentBufferOffset,
                nullptr,
                0
            );

        }
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDispatchMesh& command)
    {
        // if (!PrepareForMeshShading(command.amplificationShader, command.meshShader, command.pixelShader, command.pipelineStateObject, command.topology, command.pushConstantValues))
        // {
        //     return false;
        // }
        // commandList->GetID3D12GraphicsCommandList6()->DispatchMesh(
        //     command.threadGroupCountX,
        //     command.threadGroupCountY,
        //     command.threadGroupCountZ);
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDispatchMeshIndirect& command)
    {
        // if (!PrepareForMeshShading(command.amplificationShader, command.meshShader, command.pixelShader, command.pipelineStateObject, command.topology, command.pushConstantValues))
        // {
        //     return false;
        // }
        // commandList->GetID3D12GraphicsCommandList6()->ExecuteIndirect(
        //     device->GetDispatchMeshIndirectCommandSignature(),
        //     command.numDraws,
        //     device->GetBuffer(command.argumentBuffer)->GetID3D12Resource(),
        //     command.argumentBufferOffset,
        //     nullptr,
        //     0);
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandBeginDebugLabel& command)
    {
        PIXBeginEvent(commandList->GetID3D12GraphicsCommandList6(),0xFF000000,UTF8ToUTF16(command.labelName).c_str());
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandEndDebugLabel& command) 
    {
        PIXEndEvent(commandList->GetID3D12GraphicsCommandList6());
        return true;
    }

    bool D3D12RenderBackendCommandListContext::CompileRenderBackendCommand(const RenderBackendCommandDispatchSuperSampling& command)
    {
        // todo
        // OPTICK_EVENT();
        D3D12Texture* outputTexture = device->GetTexture(command.output);
        D3D12Texture* colorTexture = device->GetTexture(command.color);
        D3D12Texture* depthTexture = device->GetTexture(command.depth);
        D3D12Texture* motionVectorTexture = device->GetTexture(command.motionVectors);
        D3D12Texture* exposureTexture = device->GetTexture(command.exposure);

        auto GetRenderBackendTextureResourceD3D12 = [](D3D12Texture* texture, D3D12_RESOURCE_STATES state)
        {
                RenderBackendTextureResource textureResource = {};
                textureResource.texture = texture->resource.Get();
                textureResource.info = nullptr;
                textureResource.memory = nullptr;
                textureResource.view = nullptr;
                textureResource.width = texture->width;
                textureResource.height = texture->height;
                textureResource.mipLevels = texture->mipLevels;
                textureResource.arrayLayers = texture->arraySize;
                textureResource.format = texture->format;
                textureResource.state = state;
                textureResource.flags = 0;
                textureResource.usage = 0;
                return textureResource;

        };

        RenderBackendTextureResource output = GetRenderBackendTextureResourceD3D12(outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        RenderBackendTextureResource color = GetRenderBackendTextureResourceD3D12(colorTexture, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        RenderBackendTextureResource depth = GetRenderBackendTextureResourceD3D12(depthTexture, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        RenderBackendTextureResource motionVectors = GetRenderBackendTextureResourceD3D12(motionVectorTexture, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
        RenderBackendTextureResource exposure = GetRenderBackendTextureResourceD3D12(exposureTexture, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

        bool succeed = command.callback(static_cast<void*>(commandList->commandList.Get()), command.context, output, color, depth, motionVectors, exposure);
        
        // TODO: restore pipeline stateObject
        ID3D12DescriptorHeap* descriptorHeaps[] =
        {
            device->resourceDescriptorHeap->GetID3D12DescriptorHeap(),
            device->samplerDescriptorHeap->GetID3D12DescriptorHeap()
        };

        commandList->GetID3D12GraphicsCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);


        commandList->GetID3D12GraphicsCommandList6()->SetGraphicsRootSignature(device->GetID3D12RootSignature());
        commandList->GetID3D12GraphicsCommandList6()->SetComputeRootSignature(device->GetID3D12RootSignature());
       /* if (device->backend->enableHardwareRayTracing)
        {
            commandList->GetID3D12GraphicsCommandList4()->SetComputeRootSignature(device->GetID3D12RootSignature());
        }*/

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

        /*
        允许应用程序在全屏时切换显示模式（分辨率、刷新率）和硬件显示器相关。
        */
        UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


        if (backend->IsTearingSupported())
        {
            swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        }

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChain->width = desc->width;
        swapChain->height = desc->height;
        swapChainDesc.Format = ConvertToDXGIFormat(desc->format);
        swapChainDesc.Stereo = false; //是否支持立体3D(VR)
        swapChainDesc.SampleDesc.Count = 1; //MSAA采样次数
        swapChainDesc.SampleDesc.Quality = 0; //MSAA采样点分布，Quality 越高，采样点分布越优化，效果越好
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
        swapChainDesc.BufferCount = desc->numBuffers;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; //交换前后缓冲区的指针用于呈现在显示器上
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE; //交换链的 Alpha 通道要不要参与与桌面的混合？就是你的桌面画面
        swapChainDesc.Flags = swapChainFlags;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;//把画面拉伸填满整个窗口window创建的那个渲染窗口


        //全局描述
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
        fullscreenDesc.Windowed = true;//以窗口模式创建

        D3D12_CHECK(backend->GetIDXGIFactory()->CreateSwapChainForHwnd(
            commandQueue,
            windowHandle,
            &swapChainDesc,
            &fullscreenDesc,
            nullptr,//限制画面输出到哪个显示器
            &swapChain->dxgiSwapChain1
        ));

        swapChain->width = swapChainDesc.Width;
        swapChain->height = swapChainDesc.Height;
        swapChain->format = swapChainDesc.Format;
        swapChain->numBuffers = swapChainDesc.BufferCount;
        swapChain->vsyncEnabled = desc->vsync;
        swapChain->windowed = fullscreenDesc.Windowed;

        if (swapChain->dxgiSwapChain1) 
        {
            D3D12_CHECK(swapChain->dxgiSwapChain1->QueryInterface(IID_PPV_ARGS(&swapChain->dxgiSwapChain2)));
            D3D12_CHECK(swapChain->dxgiSwapChain1->QueryInterface(IID_PPV_ARGS(&swapChain->dxgiSwapChain3)));
            D3D12_CHECK(swapChain->dxgiSwapChain1->QueryInterface(IID_PPV_ARGS(&swapChain->dxgiSwapChain4)));
        }

        //设置色彩空间
        DXGI_COLOR_SPACE_TYPE colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

        UINT colorSpaceSupport = 0;
        if (SUCCEEDED(swapChain->dxgiSwapChain4->CheckColorSpaceSupport(colorSpace, &colorSpaceSupport))) 
        {
            //DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT 这个色彩空间可以用于present
            if (colorSpaceSupport& DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT)
            {
                if (SUCCEEDED(swapChain->dxgiSwapChain4->SetColorSpace1(colorSpace))) 
                {
                    switch (colorSpace)
                    {
                    default:
                    case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
                        break;
                    case DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709:
                        break;
                    case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
                        break;
                    }
                
                }
            
            }
        
        }

        for (uint32 i = 0; i < desc->numBuffers; i++) 
        {
            D3D12Texture* texture = new D3D12Texture();
            texture->width = swapChainDesc.Width;
            texture->height = swapChainDesc.Height;
            texture->arraySize = 1;
            texture->mipLevels = 1;
            texture->allocation = nullptr;
            texture->isSwapChainBuffer = true;
            D3D12_CHECK(swapChain->dxgiSwapChain4->GetBuffer(i,IID_PPV_ARGS(&texture->resource)));
            D3D12_CHECK(texture->resource->SetName(L"SwapChainBuffer"));

            texture->debugName = "SwapChainBuffer";

            uint32 textureIndex = AllocateTexture();
            textures[textureIndex] = texture;
            swapChain->buffers[i] = backend->handleManager.Allocate<RenderBackendTextureHandle>(~0u);
            SetRenderBackendHandleRepresentation(swapChain->buffers[i].GetIndex(), textureIndex);

            D3D12_CHECK(device->CreateFence(1,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&swapChain->frameFences[i])));

        }
        uint32 index = (uint32)swapChains.size();
        swapChains.emplace_back(swapChain);
        return index;
    }

    RenderBackendSwapChainHandle D3D12RenderBackend::CreateSwapChain(const RenderBackendSwapChainDesc *desc)
    {
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12SwapChain(desc);

        RenderBackendSwapChainHandle handle = handleManager.Allocate<RenderBackendSwapChainHandle>();
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }

    void D3D12RenderBackend::DestroySwapChain(RenderBackendSwapChainHandle handle) 
    {
        
    
    }

    void D3D12RenderBackend::ResizeSwapChain(RenderBackendSwapChainHandle hanlde,uint32* width,uint32* height)
    {
        D3D12Device* device = devices[0];
        D3D12SwapChain* swapChain = device->GetSwapChain(hanlde);

        device->WaitIdle();

        swapChain->width = *width;
        swapChain->height = *height;

        UINT swapChainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        if (device->backend->IsTearingSupported()) 
        {
            swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        
        }

        for (uint32 i = 0; i < swapChain->numBuffers; i++) 
        {
            D3D12Texture* texture = device->GetTexture(swapChain->buffers[i]);
            texture->resource = nullptr;
        }

        D3D12_CHECK(swapChain->GetIDXGISwapChain4()->ResizeBuffers(
            swapChain->numBuffers,
            swapChain->width,
            swapChain->height,
            swapChain->format,
            swapChainFlags
        ));

        for (uint32 i = 0; i < swapChain->numBuffers; i++) 
        {
            D3D12Texture* texture = device->GetTexture(swapChain->buffers[i]);
            texture->width = swapChain->width;
            texture->height = swapChain->height;
            D3D12_CHECK(swapChain->dxgiSwapChain4->GetBuffer(i, IID_PPV_ARGS(&texture->resource)));
            D3D12_CHECK(texture->resource->SetName(L"SwapChainBuffer"));
        }
    }

    bool D3D12RenderBackend::PresentSwapChain(RenderBackendSwapChainHandle handle) 
    {   //todo
        //OPTICK_EVENT();

        D3D12Device* device = devices[0];
        D3D12SwapChain* swapChain = device->GetSwapChain(handle);

        const UINT presentSyncInterval = swapChain->vsyncEnabled ? 1 : 0;
    
        UINT presentFlags = 0;
        if (IsTearingSupported() && swapChain->windowed && presentSyncInterval == 0) 
        {
            presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
        }

        HRESULT hr = swapChain->GetIDXGISwapChain4()->Present(presentSyncInterval,presentFlags);
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) 
        {
            return false;
        }

        UINT lastPresentCount = 0;
        if (SUCCEEDED(swapChain->GetIDXGISwapChain4()->GetLastPresentCount(&lastPresentCount)))
        {
            //presentCounter = lastPresentCount;
        }
        else
        {
            //presentCounter++;
        }

        // TODO: refactor this
        {
            uint32 bufferIndex = swapChain->GetCurrentBackBufferIndex();
            ID3D12Fence* fence = swapChain->GetFrameFence(bufferIndex);
            if (fence->GetCompletedValue() < 1)
            {
                // If hEvent is a null handle, then this API will not return until the specified fence value(s) have been reached.
                hr = fence->SetEventOnCompletion(1, NULL);
                assert(SUCCEEDED(hr));
            }
            hr = fence->Signal(0);
            assert(SUCCEEDED(hr));
        }

        return true;
    }

    RenderBackendTextureHandle D3D12RenderBackend::GetActiveSwapChainBuffer(RenderBackendSwapChainHandle handle) 
    {
        D3D12Device* device = devices[0];
        D3D12SwapChain* swapChain = device->GetSwapChain(handle);
        return swapChain->buffers[swapChain->GetCurrentBackBufferIndex()];
    }

    RenderBackendBufferHandle D3D12RenderBackend::CreateBuffer(const RenderBackendBufferDescription* desc,const void* data,const char* name) 
    {
        RenderBackendBufferHandle handle = handleManager.Allocate<RenderBackendBufferHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12Buffer(desc, data, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }

    void D3D12RenderBackend::ResizeBuffer(RenderBackendBufferHandle handle,uint64 size) 
    {
        D3D12Device* device = devices[0];
        uint32 index = 0;
        if (device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(),&index)) 
        {
            device->ResizeD3D12Buffer(index, size);
        }
    }

    void D3D12RenderBackend::MapBuffer(RenderBackendBufferHandle handle,void** data) 
    {
        D3D12Device* device = devices[0];
        D3D12Buffer* buffer = device->GetBuffer(handle);

        /*
            HRESULT ID3D12Resource::Map(
                UINT            Subresource,   // 子资源索引
                const D3D12_RANGE* pReadRange, // CPU 要读取的范围
                void**          ppData         // 输出：CPU 可访问的指针
            );
        
            子资源索引:
                对于 Buffer（缓冲区）：永远是 0
                Buffer 只有一个子资源，没有 mip level、没有 array slice
                对于 Texture（纹理）：可以是 0, 1, 2...
                每个 mip level、每个 array slice 都是一个子资源
                
                例如一个 3 级 mipmap 的纹理：
                子资源 0 = mip 0（最大）
                子资源 1 = mip 1（中等）
                子资源 2 = mip 2（最小）


            nullptr 的含义：
                "CPU 可能会读取整个缓冲区的任何位置"
                驱动会确保整个缓冲区对 CPU 可读

                如果传具体范围：
                D3D12_RANGE range = { 0, 0 };  // Begin=0, End=0
                buffer->resource->Map(0, &range, &data);

                range { 0, 0 } 的含义：
                "CPU 不会读取任何数据，只会写入"
                驱动可以跳过 GPU→CPU 的缓存同步
                
           传入 void**:
                Map 会把 CPU 可访问的地址写进去
                Map 之后：data 指向一块 CPU 可以直接访问的内存
                这块内存和 GPU 的缓冲区是"映射"关系
        */

        buffer->resource->Map(0, nullptr, data);
    
    }

    void D3D12RenderBackend::UnmapBuffer(RenderBackendBufferHandle handle) 
    {
        D3D12Device* device = devices[0];
        D3D12Buffer* buffer = device->GetBuffer(handle);
        /*
            三种传法：
              ┌─────────────────────────────────────────────────────────┐
              │  nullptr                                                │
              │                                                         │
              │  "我可能修改了整个缓冲区的任何位置"                         │
              │                                                         │
              │  驱动会刷新整个缓冲区                                     │
              │  最安全，但性能不是最优                                    │
              └─────────────────────────────────────────────────────────┘

              ┌─────────────────────────────────────────────────────────┐
              │  D3D12_RANGE { Begin, End }                             │
              │                                                         │
              │  "我只修改了 [Begin, End) 这个范围"                       │
              │                                                         │
              │  驱动只刷新这个范围                                       │
              │  性能更好                                             │
              └─────────────────────────────────────────────────────────┘

              ┌─────────────────────────────────────────────────────────┐
              │  D3D12_RANGE { 0, 0 }                                   │
              │                                                         │
              │  "我没有修改任何东西"（只读场景）                           │
              │                                                         │
              │   驱动不需要刷新任何数据                                    │
              │   性能最好（用于 Readback 堆只读的情况）                  │
              └─────────────────────────────────────────────────────────┘
        */
        buffer->resource->Unmap(0, nullptr);
    }

    void D3D12RenderBackend::DestroyBuffer(RenderBackendBufferHandle handle)
    {

    }

    RenderBackendTextureHandle D3D12RenderBackend::CreateTexture(const RenderBackendTextureDesc* desc,const void* data,const char* name) 
    {
        RenderBackendTextureHandle handle = handleManager.Allocate<RenderBackendTextureHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12Texture(desc, data, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(),index);
        return handle;
    }

    void D3D12RenderBackend::DestroyTexture(RenderBackendTextureHandle texture)
    {

    }

    void D3D12RenderBackend::UploadTexture(RenderBackendTextureHandle handle, const RenderBackendTextureUploadDataDesc& data)
    {

    }

    void D3D12RenderBackend::GetTextureReadbackData(RenderBackendTextureHandle texture, void** data)
    {

    }


    RenderBackendTextureViewHandle D3D12RenderBackend::CreateTextureView(
        RenderBackendTextureHandle textureHandle,
        const RenderBackendTextureViewDesc* desc,
        int32* descriptor
    ) 
    {
        D3D12Device* device = devices[0];
        D3D12Texture* texture = device->GetTexture(textureHandle);

        if (desc->IsRenderTargetView()) 
        {   
            D3D12RenderTargetView* textureView = new D3D12RenderTargetView();
            textureView->descriptor = device->rtvDescriptorAllocator.Allocate();

            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
            rtvDesc.Format = texture->format;

            switch (texture->t)
            {
            case RenderBackendTextureType::Texture1D:
            {
                if (texture->arraySize == 1)
                {
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                    rtvDesc.Texture1D.MipSlice = desc->subresourceRange.firstLevel;

                }
                else
                {
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
                    rtvDesc.Texture1DArray.MipSlice = desc->subresourceRange.firstLevel;
                    rtvDesc.Texture1DArray.FirstArraySlice = desc->subresourceRange.firstLayer;
                    rtvDesc.Texture1DArray.ArraySize = desc->subresourceRange.arrayLayers;
                }
            }break;
            case RenderBackendTextureType::Texture2D:
            {
                if (texture->arraySize == 1)
                {
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                    rtvDesc.Texture2D.MipSlice = desc->subresourceRange.firstLevel;
                    rtvDesc.Texture2D.PlaneSlice = 0;
                }
                else
                {
                    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                    rtvDesc.Texture2DArray.MipSlice = desc->subresourceRange.firstLevel;
                    rtvDesc.Texture2DArray.FirstArraySlice = desc->subresourceRange.firstLayer;
                    rtvDesc.Texture2DArray.ArraySize = desc->subresourceRange.arrayLayers;
                    rtvDesc.Texture2DArray.PlaneSlice = 0;
                }
            }break;
            case RenderBackendTextureType::Texture3D: 
            {
                rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                rtvDesc.Texture3D.MipSlice = desc->subresourceRange.firstLevel;
                rtvDesc.Texture3D.FirstWSlice = 0;
                rtvDesc.Texture3D.WSize = -1;//使用从 FirstWSlice 到该 mip 深度末尾的所有 slice

            }break;
            default:
                std::abort();
                break;
            }

            device->GetID3D12Device()->CreateRenderTargetView(texture->GetID3D12Resource(),&rtvDesc, textureView->descriptor);
            return reinterpret_cast<RenderBackendTextureViewHandle>(textureView);
        }

        return nullptr;

    }
    
    int32 D3D12RenderBackend::GetTextureSRVBindlessResourceDescriptorIndex(RenderBackendTextureHandle handle) 
    {
        D3D12Device*& device = devices[0];
        uint32 textureIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &textureIndex)) 
        {
            return 0;
        }
        D3D12Texture* texture = device->textures[textureIndex];
        return texture->shaderResourceView->bindlessIndex;
    }

    int32 D3D12RenderBackend::GetTextureSRVBindlessResourceDescriptorIndex(RenderBackendTextureHandle handle,uint32 mipLevel) 
    {
        D3D12Device* device = devices[0];
        uint32 textureIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &textureIndex))
        {
            return 0;
        }
        D3D12Texture* texture = device->textures[textureIndex];
        return texture->shaderResourceViews[mipLevel]->bindlessIndex;
    }

    int32 D3D12RenderBackend::GetTextureUAVBindlessResourceDescriptorIndex(RenderBackendTextureHandle handle,uint32 mipLevel) 
    {
        D3D12Device* device = devices[0];
        uint32 textureIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &textureIndex)) 
        {
            return 0;
        }
        D3D12Texture* texture = device->textures[textureIndex];
        return texture->GetUnorderedAccessView(mipLevel)->bindlessIndex;
    }

    int32 D3D12RenderBackend::GetBufferCBVBindlessResourceDescriptorIndex(RenderBackendBufferHandle handle) 
    {
        D3D12Device* device = devices[0];
        uint32 bufferIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &bufferIndex))
        {
            return 0;
        }
        D3D12Buffer* buffer = device->buffers[bufferIndex];
        return buffer->bindlessResourceDescriptorIndexCBV;
    }

    int32 D3D12RenderBackend::GetBufferSRVBindlessResourceDescriptorIndex(RenderBackendBufferHandle handle)
    {
        D3D12Device* device = devices[0];
        uint32 bufferIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &bufferIndex)) 
        {
            return 0;
        }

        D3D12Buffer* buffer = device->buffers[bufferIndex];
        return buffer->bindlessResourceDescriptorIndexSRV;
    }

    int32 D3D12RenderBackend::GetBufferUAVBindlessResourceDescriptorIndex(RenderBackendBufferHandle handle) 
    {
        D3D12Device* device = devices[0];
        uint32 bufferIndex = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &bufferIndex)) 
        {
            return 0;
        }
        D3D12Buffer* buffer = device->buffers[bufferIndex];
        return buffer->bindlessResourceDescriptorIndexUAV;
    }


   /* int32 D3D12RenderBackend::GetAccelerationStructureSRVBindlessResourceDescriptorIndex(RenderBackendRayTracingAccelerationStructureHandle handle)
    {
        D3D12Device* device = devices[0];
        uint32 index = 0;
        if (!device->TryGetRenderBackendHandleRepresentation(handle.GetIndex(), &index))
        {
            return 0;
        }
        D3D12RayTracingAccelerationStructure* accelerationStructure = device->accelerationStructures[index];
        return accelerationStructure->bindlessIndex;
    }*/

    RenderBackendSamplerHandle D3D12RenderBackend::CreateSampler(const RenderBackendSamplerDesc* desc , const char* name) 
    {
        RenderBackendSamplerHandle handle = handleManager.Allocate<RenderBackendSamplerHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12Sampler(desc,name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(),index);
        return handle;
    }

    void D3D12RenderBackend::DestroySampler(RenderBackendSamplerHandle sampler)
    {

    }

    RenderBackendShaderHandle D3D12RenderBackend::CreateShader(const RenderBackendShaderDesc* desc, const char* name) 
    {
        RenderBackendShaderHandle handle = handleManager.Allocate<RenderBackendShaderHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12Shader(desc,name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(),index);
        return handle;
    }

    void D3D12RenderBackend::DestroyShader(RenderBackendShaderHandle shader)
    {

    }

    RenderBackendTimingQueryHeapHandle D3D12RenderBackend::CreateTimingQueryHeap(const RenderBackendTimingQueryHeapDesc* desc, const char* name)
    {
        RenderBackendTimingQueryHeapHandle handle = handleManager.Allocate<RenderBackendTimingQueryHeapHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12QueryHeap(desc, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }

    void D3D12RenderBackend::DestroyTimingQueryHeap(RenderBackendTimingQueryHeapHandle timingQueryHeap)
    {

    }

    void D3D12RenderBackend::SubmitCommandLists(RenderBackendCommandList** commandLists, uint32 numCommandLists, RenderBackendSwapChainHandle swapChainHandle)
    {
        //todo
        //OPTICK_EVENT();

        if (!commandLists || numCommandLists == 0)
        {
            return;
        }
        D3D12Device* device = devices[0];
        D3D12CommandQueueType queueType = D3D12CommandQueueType::Direct;

        D3D12CommandAllocator* commandAllocator = device->AllocateCommandAllocator(queueType);
        D3D12CommandList* commandList = device->AllocateCommandList(commandAllocator);

        ID3D12GraphicsCommandList6* graphicsCommandList6 = commandList->GetID3D12GraphicsCommandList6();

        ID3D12DescriptorHeap* descriptorHeaps[] =
        {
            device->resourceDescriptorHeap->GetID3D12DescriptorHeap(),
            device->samplerDescriptorHeap->GetID3D12DescriptorHeap(),
        };

        graphicsCommandList6->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        commandList->GetID3D12GraphicsCommandList6()->SetGraphicsRootSignature(device->GetID3D12RootSignature());
        commandList->GetID3D12GraphicsCommandList6()->SetComputeRootSignature(device->GetID3D12RootSignature());
        /* if (device->backend->enableHardwareRayTracing)
         {
             commandList->GetID3D12GraphicsCommandList4()->SetComputeRootSignature(device->GetID3D12RootSignature());
         }*/

        for (uint32 i = 0; i < numCommandLists; i++)
        {
            D3D12RenderBackendCommandListContext context(device, queueType, commandList);
            if (!context.CompileRenderBackendCommands(*commandLists[i]->GetCommandContainer()))
            {
                // TODO
            }

        }

        graphicsCommandList6->Close();

        D3D12CommandQueue* commandQueue = device->GetCommandQueue(queueType);

        D3D12SubmissionWorkload* workload = new D3D12SubmissionWorkload();

        workload->commandQueue = commandQueue;
        workload->commandListsToExecute.push_back(commandList);
        workload->commandAllocatorsToRelease.push_back(commandAllocator);

        if (swapChainHandle)
        {
            D3D12SwapChain* swapChain = device->GetSwapChain(swapChainHandle);
            uint32 currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
            ID3D12Fence* frameFence = swapChain->GetFrameFence(currentBackBufferIndex);

            workload->fencesToWait.push_back(D3D12SubmissionWorkload::Fence(frameFence, 0));
            workload->fencesToSignal.push_back(D3D12SubmissionWorkload::Fence(frameFence, 1));
        }

        for (auto& [fence, value] : workload->fencesToWait)
        {
            D3D12_CHECK(commandQueue->GetID3D12CommandQueue()->Wait(fence.Get(), value));//GPU：在执行后续命令前，等 fence 的值 ≥ value
        }

        const uint32 numCommandListsToExecute = (uint32)workload->commandListsToExecute.size();
        if (numCommandListsToExecute > 0)
        {
            std::vector<ID3D12CommandList*> d3d12CommandLists;

            for (D3D12CommandList* commandList : workload->commandListsToExecute)
            {
                d3d12CommandLists.push_back(commandList->GetID3D12CommandList());
            }

            commandQueue->GetID3D12CommandQueue()->ExecuteCommandLists(numCommandListsToExecute, d3d12CommandLists.data());

            for (D3D12CommandList* commandList : workload->commandListsToExecute)
            {
                device->ReleaseCommandList(commandList);
            }
        }
        uint64 fenceValue = commandQueue->SignalFence();
        workload->completionFenceValue = fenceValue;

        for (auto& [fence, value] : workload->fencesToSignal)
        {
            D3D12_CHECK(commandQueue->GetID3D12CommandQueue()->Signal(fence.Get(),value));
        }

        device->workloads.push_back(workload);

    }

   /* RenderBackendRayTracingAccelerationStructureHandle D3D12RenderBackend::CreateRayTracingBottomLevelAccelerationStructure(const RenderBackendRayTracingBottomLevelAccelerationStructureDesc* desc, const char* name)
    {
        RenderBackendRayTracingAccelerationStructureHandle handle = handleManager.Allocate<RenderBackendRayTracingAccelerationStructureHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12RayTracingBottomLevelAccelerationStructure(desc, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }*/

    //RenderBackendRayTracingAccelerationStructureHandle D3D12RenderBackend::CreateRayTracingTopLevelAccelerationStructure(const RenderBackendRayTracingTopLevelAccelerationStructureDesc* desc, const char* name)
    //{
    //    RenderBackendRayTracingAccelerationStructureHandle handle = handleManager.Allocate<RenderBackendRayTracingAccelerationStructureHandle>();
    //    D3D12Device* device = devices[0];
    //    uint32 index = device->CreateD3D12RayTracingTopLevelAccelerationStructure(desc, name);
    //    device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
    //    return handle;
    //}

   /* RenderBackendRayTracingPipelineStateHandle D3D12RenderBackend::CreateRayTracingPipelineState(const RenderBackendRayTracingPipelineStateDesc* desc, const char* name)
    {
        RenderBackendRayTracingPipelineStateHandle handle = handleManager.Allocate<RenderBackendRayTracingPipelineStateHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12RayTracingPipelineState(desc, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }

    RenderBackendBufferHandle D3D12RenderBackend::CreateRayTracingShaderBindingTable(const RenderBackendRayTracingShaderBindingTableDesc* desc, const char* name)
    {
        RenderBackendBufferHandle handle = handleManager.Allocate<RenderBackendBufferHandle>();
        D3D12Device* device = devices[0];
        uint32 index = device->CreateD3D12ShaderBindingTable(desc, name);
        device->SetRenderBackendHandleRepresentation(handle.GetIndex(), index);
        return handle;
    }*/

    bool D3D12Device::Init(D3D12RenderBackend* backend, D3D12Adapter* adapter) 
    {
        D3D_FEATURE_LEVEL minimumFeatureLevel = D3D_FEATURE_LEVEL_12_2;

        /*
            开始：D3D12 Device 对象（内部包含 InfoQueue 功能）
            device 被创建时：
            refCount = 1     （创建者持有一个引用）
        */
        HRESULT hr = D3D12CreateDevice(adapter->GetIDXGIAdapter(), minimumFeatureLevel, IID_PPV_ARGS(&device));
        if (FAILED(hr)) 
        {
            MIKU_CORE_ERROR("Error: Failed to create D3D12 device.");
            return false;
        }

        mask = { 0U };

        CD3DX12FeatureSupport features;
        D3D12_CHECK(features.Init(device.Get()));

        if (backend->useDebugLayers) 
        {
            ID3D12InfoQueue1* d3d12InfoQueue;
            /*
                 device->QueryInterface(&d3d12InfoQueue):
                 refCount = 2     （现在有两个指针指向同一个对象）
                      ├─ device            → 指向对象
                      └─ d3d12InfoQueue    → 也指向同一个对象（的 InfoQueue 接口）
            */
            if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&d3d12InfoQueue))))
            {
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, true);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_MESSAGE, true);

                std::vector<D3D12_MESSAGE_SEVERITY> enabledSeverities;

                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_CORRUPTION);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_ERROR);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_WARNING);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_INFO);
                enabledSeverities.push_back(D3D12_MESSAGE_SEVERITY_MESSAGE);
            
                std::vector<D3D12_MESSAGE_ID> disabledMessages;

                D3D12_INFO_QUEUE_FILTER filter = {};

                filter.AllowList.NumSeverities = static_cast<UINT>(enabledSeverities.size());
                filter.AllowList.pSeverityList = enabledSeverities.data();
                filter.DenyList.NumIDs = static_cast<UINT>(disabledMessages.size());
                filter.DenyList.pIDList = disabledMessages.data();
                D3D12_CHECK(d3d12InfoQueue->AddStorageFilterEntries(&filter));

                DWORD messageCallbackCookie = 0;

                //D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS 前面代码设置了 AddStorageFilterEntries 过滤器（只存储特定严重级别的消息到队列），但回调不受这个过滤器限制，所有消息都会触发回调。
                //cookie 是一个"回执号"。注册回调后，D3D12 会填入一个唯一的数字到这个变量。将来如果你想取消注册，就把这个 cookie 传给 UnregisterMessageCallback(cookie)。
                if (SUCCEEDED(d3d12InfoQueue->RegisterMessageCallback(D3D12MessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &messageCallbackCookie))) 
                {
                
                }
                else 
                {
                    assert(false);
                }

                d3d12InfoQueue->Release();
                /*
                * Release() → refCount--，如果变成 0 就 delete this
                    COM 引用计数 - 1，释放接口指针，但回调仍有效
                */
            }
        }

        D3D_FEATURE_LEVEL maxSupportedFeatureLevel = minimumFeatureLevel;

        constexpr D3D_FEATURE_LEVEL allFeatureLevels[] =
        {
            D3D_FEATURE_LEVEL_12_2,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
            D3D_FEATURE_LEVEL_1_0_CORE,
            D3D_FEATURE_LEVEL_1_0_GENERIC
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS  featureDataFeatureLevels =
        {
            _countof(allFeatureLevels),
            allFeatureLevels,
        };

        /*
            D3D12_FEATURE_FEATURE_LEVELS  要查询的特性类别（枚举值）
        */
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureDataFeatureLevels, sizeof(featureDataFeatureLevels)))) 
        {
            maxSupportedFeatureLevel = featureDataFeatureLevels.MaxSupportedFeatureLevel;
        }

        MIKU_CORE_INFO("Max supported feature level {0}.", GetD3DFeatureLevel(maxSupportedFeatureLevel));

        ///询问 GPU 的内存架构类型
        D3D12_FEATURE_DATA_ARCHITECTURE featureDataArchitecture = {};
        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE,&featureDataArchitecture,sizeof(featureDataArchitecture))))
        {
        
        
        }

        D3D12_FEATURE_DATA_SHADER_MODEL featureDataShaderModel =
        {
            D3D_SHADER_MODEL_NONE
        };

        constexpr D3D_SHADER_MODEL allShaderModels[] =
        {
            D3D_SHADER_MODEL_6_9,
            D3D_SHADER_MODEL_6_8,
            D3D_SHADER_MODEL_6_7,
            D3D_SHADER_MODEL_6_6,
            //D3D_SHADER_MODEL_6_5,
            //D3D_SHADER_MODEL_6_4,
            //D3D_SHADER_MODEL_6_3,
            //D3D_SHADER_MODEL_6_2,
            //D3D_SHADER_MODEL_6_1,
            //D3D_SHADER_MODEL_6_0,
            //D3D_SHADER_MODEL_5_1,
        };

        for (const D3D_SHADER_MODEL shaderModel : allShaderModels)
        {
            featureDataShaderModel.HighestShaderModel = shaderModel;
            if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &featureDataShaderModel, sizeof(featureDataShaderModel))))
            {
                break;
            }
        }

        if (featureDataShaderModel.HighestShaderModel == D3D_SHADER_MODEL_NONE)
        {
            // TODO
        }

        MIKU_CORE_INFO("Max supported shader model{}.", GetD3DShaderModel(featureDataShaderModel.HighestShaderModel));

        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        D3D12_CHECK(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)));

        // See: https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html
       // ResourceDescriptorHeap/SamplerDescriptorHeap must be supported on devices that support both D3D12_RESOURCE_BINDING_TIER_3 and D3D_SHADER_MODEL_6_6
        if (options.ResourceBindingTier >= D3D12_RESOURCE_BINDING_TIER_3 && featureDataShaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_6) 
        {
            MIKU_CORE_INFO("Bindless resources are supported.");
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));

        if (options5.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
        {
            MIKU_CORE_INFO("DirectX Raytracing (DXR) is not supported.");
        }
        else
        {
            MIKU_CORE_INFO("DirectX Raytracing (DXR) is supported. Tier: {}.", (int)options5.RaytracingTier);

            device->QueryInterface(IID_PPV_ARGS(&device5));
        }


        D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7));

        if (options7.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
        {
            MIKU_CORE_INFO("Mesh shader is not supported.");
        }
        else
        {
            MIKU_CORE_INFO("Mesh shader is supported. Tier: {}.", (int)options7.MeshShaderTier);
            // dispatchRaysIndirect = true;
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS9 options9 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &options9, sizeof(options9));

        D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS11, &options11, sizeof(options11));

        D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12));

        if (options12.EnhancedBarriersSupported != TRUE)
        {
            MIKU_CORE_INFO("Enhanced Barriers are not supported.");
        }
        else
        {
            MIKU_CORE_INFO("Enhanced Barriers are supported.");

            device->QueryInterface(IID_PPV_ARGS(&device10));
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS18 options18 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS18, &options18, sizeof(options18));

        if (options18.RenderPassesValid != TRUE)
        {
            MIKU_CORE_INFO("Render Pass is not supported.");
        }
        else
        {
            MIKU_CORE_INFO("Render Pass is supported. Tier: {}.", GetD3D12RenderPassesTierName(options5.RenderPassesTier));
        }

        D3D12_FEATURE_DATA_D3D12_OPTIONS21 options21 = {};
        device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS21, &options21, sizeof(options21));

        if (options21.WorkGraphsTier == D3D12_WORK_GRAPHS_TIER_NOT_SUPPORTED)
        {
            MIKU_CORE_INFO("Work graphs are not supported.");
        }
        else
        {
            MIKU_CORE_INFO("Work graphs are supported. Tier: {}.", (int)options21.WorkGraphsTier);
        }

        for (uint32 i = 0 ; i < (uint32)D3D12CommandQueueType::Count;i++) 
        {
            commandQueues[i] = nullptr;
        }

        {
            D3D12CommandQueueType queueType = D3D12CommandQueueType::Direct;

            D3D12CommandQueue* commandQueue = new D3D12CommandQueue();
            commandQueue->queueType = queueType;

            D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
            commandQueueDesc.Type = GetD3D12CommandListType(queueType);
            commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; //此对列的渲染优先级
            commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            commandQueueDesc.NodeMask = mask.Get();

            D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc,IID_PPV_ARGS(&commandQueue->queue)));
            D3D12_CHECK(commandQueue->queue->SetName(L"3D Queue"));
        
            D3D12_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&commandQueue->fence)));
            D3D12_CHECK(commandQueue->fence->SetName(L"3D Queue Fence"));

            commandQueue->lastSignaledValue = 0;

            commandQueues[(uint32)queueType] = commandQueue;
        }
        {
            D3D12CommandQueueType queueType = D3D12CommandQueueType::Compute;

            D3D12CommandQueue* commandQueue = new D3D12CommandQueue();
            commandQueue->queueType = queueType;


            D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {
                GetD3D12CommandListType(queueType),
                D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                D3D12_COMMAND_QUEUE_FLAG_NONE,
                mask.Get(),
            };
            D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue->queue)));
            D3D12_CHECK(commandQueue->queue->SetName(L"Compute Queue"));

            D3D12_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&commandQueue->fence)));
            D3D12_CHECK(commandQueue->fence->SetName(L"Compute Queue Fence"));

            commandQueue->lastSignaledValue = 0;

            commandQueues[(uint32)queueType] = commandQueue;
        }

        {
            D3D12CommandQueueType queueType = D3D12CommandQueueType::Copy;

            D3D12CommandQueue* commandQueue = new D3D12CommandQueue();
            commandQueue->queueType = queueType;

            D3D12_COMMAND_QUEUE_DESC commandQueueDesc =
            {
               GetD3D12CommandListType(queueType),
               D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
               D3D12_COMMAND_QUEUE_FLAG_NONE,
               mask.Get(),
            };

            D3D12_CHECK(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue->queue)));
            D3D12_CHECK(commandQueue->queue->SetName(L"Copy Queue"));

            D3D12_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&commandQueue->fence)));
            D3D12_CHECK(commandQueue->fence->SetName(L"Copy Queue Fence"));

            commandQueue->lastSignaledValue = 0;

            commandQueues[(uint32)queueType] = commandQueue;
        }
            //Create command signatures
        {
                D3D12_INDIRECT_ARGUMENT_DESC drawIndirectArgumentDesc = {};
                drawIndirectArgumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.ByteStride = sizeof(D3D12_DRAW_ARGUMENTS);
                commandSignatureDesc.NumArgumentDescs = 1;
                commandSignatureDesc.pArgumentDescs = &drawIndirectArgumentDesc;
                commandSignatureDesc.NodeMask = 0;
                D3D12_CHECK(device->CreateCommandSignature(&commandSignatureDesc,nullptr, IID_PPV_ARGS(&drawIndirectCommandSignature)));
        }

        {
                D3D12_INDIRECT_ARGUMENT_DESC drawIndexedIndirectArgumentDesc = {};
                drawIndexedIndirectArgumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.ByteStride = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
                commandSignatureDesc.NumArgumentDescs = 1;
                commandSignatureDesc.pArgumentDescs = &drawIndexedIndirectArgumentDesc;
                commandSignatureDesc.NodeMask = 0;
                D3D12_CHECK(device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(&drawIndexedIndirectCommandSignature)));
            
        }
        
        {
                D3D12_INDIRECT_ARGUMENT_DESC dispatchIndirectArgumentDesc = {};
                dispatchIndirectArgumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);
                commandSignatureDesc.NumArgumentDescs = 1;
                commandSignatureDesc.pArgumentDescs = &dispatchIndirectArgumentDesc;
                commandSignatureDesc.NodeMask = 0;
                D3D12_CHECK(device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(&dispatchIndirectCommandSignature)));
        }
            // TODO: if (supportDispatchRaysIndirect)
        {
                D3D12_INDIRECT_ARGUMENT_DESC dispatchRaysIndirectArgumentDesc = {};
                dispatchRaysIndirectArgumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_RAYS;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.ByteStride = sizeof(D3D12_DISPATCH_RAYS_DESC);
                commandSignatureDesc.NumArgumentDescs = 1;
                commandSignatureDesc.pArgumentDescs = &dispatchRaysIndirectArgumentDesc;
                commandSignatureDesc.NodeMask = 0;
                D3D12_CHECK(device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(&dispatchRaysIndirectCommandSignature)));
        }
            //  TODO: if (supportMeshShading)
        {
                D3D12_INDIRECT_ARGUMENT_DESC dispatchMeshIndirectArgumentDesc = {};
                dispatchMeshIndirectArgumentDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;

                D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
                commandSignatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);
                commandSignatureDesc.NumArgumentDescs = 1;
                commandSignatureDesc.pArgumentDescs = &dispatchMeshIndirectArgumentDesc;
                commandSignatureDesc.NodeMask = 0;
                D3D12_CHECK(device->CreateCommandSignature(&commandSignatureDesc, nullptr, IID_PPV_ARGS(&dispatchMeshIndirectCommandSignature)));
        }

            resourceDescriptorAllocator.Init(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8192);
            samplerDescriptorAllocator.Init(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 256);
            rtvDescriptorAllocator.Init(this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024);
            dsvDescriptorAllocator.Init(this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 128);

            // Resource descriptor heap
        {
                resourceDescriptorHeap = new D3D12DescriptorHeap();

                D3D12_DESCRIPTOR_HEAP_DESC heapDesc =
                {
                    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                    1000000,
                    D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                    mask.Get()

                };

                D3D12_CHECK(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&resourceDescriptorHeap->descriptorHeap)));
                D3D12_CHECK(resourceDescriptorHeap->descriptorHeap->SetName(L"BindlessResourceDescriptorHeap"));

                resourceDescriptorHeap->cpuDescriptorHandle = resourceDescriptorHeap->GetID3D12DescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
                resourceDescriptorHeap->gpuDescriptorHandle = resourceDescriptorHeap->GetID3D12DescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
                
                for (int i = 0; i < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS; i++) 
                {
                    freeResourceDescriptorIndices.push_back(D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_RESOURCE_DESCRIPTORS - i - 1);
                }
        }

            // Sampler descriptor heap
        {
            
                samplerDescriptorHeap = new D3D12DescriptorHeap();
                D3D12_DESCRIPTOR_HEAP_DESC heapDesc =
                {
                    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
                    2048,
                    D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                    mask.Get(),
                };

                D3D12_CHECK(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerDescriptorHeap->descriptorHeap)));
                D3D12_CHECK(samplerDescriptorHeap->descriptorHeap->SetName(L"BindlessSamplerDescriptorHeap"));
            
                samplerDescriptorHeap->cpuDescriptorHandle = samplerDescriptorHeap->GetID3D12DescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
                samplerDescriptorHeap->gpuDescriptorHandle = samplerDescriptorHeap->GetID3D12DescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

                for (int i = 0; i < D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_SAMPLER_DESCRIPTOERS; i++)
                {
                    freeSamplerDescriptorIndices.push_back(D3D12_RENDER_BACKEND_BINDLESS_MAX_NUM_SAMPLER_DESCRIPTOERS - i - 1);
                }
        }
        CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[2] = {};
        descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, -1, 0, 1, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, 0);
        descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, -1, 0, 2, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, 0);
        
        CD3DX12_ROOT_PARAMETER1 rootParameters[2] = {};
        rootParameters[0].InitAsConstants(RenderBackendPushConstantsBytes / 4, 999, 0, D3D12_SHADER_VISIBILITY_ALL);
        rootParameters[1].InitAsDescriptorTable(2, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);

        //D3D12 SM6.6 Bindless（无绑定）资源访问 模式的根签名标志。
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        //允许着色器通过索引直接访问 CBV/SRV/UAV 描述符堆中的任意资源。
        rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        //允许着色器通过索引直接访问 Sampler 描述符堆中的任意采样器。
        rootSignatureFlags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

        D3D12_STATIC_SAMPLER_DESC staticSamplers[9] = {};
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreatePointWarp(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[0].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[0].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[0].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[0].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[0].MipLODBias = desc.mipLodBias;
            staticSamplers[0].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[0].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[0].MinLOD = desc.minLod;
            staticSamplers[0].MaxLOD = desc.maxLod;
            staticSamplers[0].ShaderRegister = 100;
            staticSamplers[0].RegisterSpace = 0;
            staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreatePointClamp(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[1].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[1].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[1].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[1].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[1].MipLODBias = desc.mipLodBias;
            staticSamplers[1].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[1].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[1].MinLOD = desc.minLod;
            staticSamplers[1].MaxLOD = desc.maxLod;
            staticSamplers[1].ShaderRegister = 101;
            staticSamplers[1].RegisterSpace = 0;
            staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreatePointBorder(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[2].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[2].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[2].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[2].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[2].MipLODBias = desc.mipLodBias;
            staticSamplers[2].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[2].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[2].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[2].MinLOD = desc.minLod;
            staticSamplers[2].MaxLOD = desc.maxLod;
            staticSamplers[2].ShaderRegister = 102;
            staticSamplers[2].RegisterSpace = 0;
            staticSamplers[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateLinearWarp(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[3].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[3].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[3].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[3].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[3].MipLODBias = desc.mipLodBias;
            staticSamplers[3].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[3].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[3].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[3].MinLOD = desc.minLod;
            staticSamplers[3].MaxLOD = desc.maxLod;
            staticSamplers[3].ShaderRegister = 103;
            staticSamplers[3].RegisterSpace = 0;
            staticSamplers[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateLinearClamp(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[4].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[4].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[4].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[4].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[4].MipLODBias = desc.mipLodBias;
            staticSamplers[4].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[4].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[4].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[4].MinLOD = desc.minLod;
            staticSamplers[4].MaxLOD = desc.maxLod;
            staticSamplers[4].ShaderRegister = 104;
            staticSamplers[4].RegisterSpace = 0;
            staticSamplers[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateLinearBorder(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[5].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[5].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[5].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[5].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[5].MipLODBias = desc.mipLodBias;
            staticSamplers[5].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[5].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[5].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[5].MinLOD = desc.minLod;
            staticSamplers[5].MaxLOD = desc.maxLod;
            staticSamplers[5].ShaderRegister = 105;
            staticSamplers[5].RegisterSpace = 0;
            staticSamplers[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateLinearMirror(0.0f, -FLT_MAX, FLT_MAX, 1);
            staticSamplers[6].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[6].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[6].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[6].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[6].MipLODBias = desc.mipLodBias;
            staticSamplers[6].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[6].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[6].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[6].MinLOD = desc.minLod;
            staticSamplers[6].MaxLOD = desc.maxLod;
            staticSamplers[6].ShaderRegister = 106;
            staticSamplers[6].RegisterSpace = 0;
            staticSamplers[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateComparisonLinearClamp(0.0f, -FLT_MAX, FLT_MAX, 0, RenderBackendCompareOp::Greater);
            staticSamplers[7].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[7].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[7].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[7].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[7].MipLODBias = desc.mipLodBias;
            staticSamplers[7].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[7].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[7].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[7].MinLOD = desc.minLod;
            staticSamplers[7].MaxLOD = desc.maxLod;
            staticSamplers[7].ShaderRegister = 107;
            staticSamplers[7].RegisterSpace = 0;
            staticSamplers[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        {
            RenderBackendSamplerDesc desc = RenderBackendSamplerDesc::CreateComparisonLinearClamp(0.0f, -FLT_MAX, FLT_MAX, 1, RenderBackendCompareOp::Less);
            staticSamplers[8].Filter = ConvertToD3D12Filter(desc.filter);
            staticSamplers[8].AddressU = ConvertToD3D12TextureAddressMode(desc.addressModeU);
            staticSamplers[8].AddressV = ConvertToD3D12TextureAddressMode(desc.addressModeV);
            staticSamplers[8].AddressW = ConvertToD3D12TextureAddressMode(desc.addressModeW);
            staticSamplers[8].MipLODBias = desc.mipLodBias;
            staticSamplers[8].MaxAnisotropy = desc.maxAnisotropy;
            staticSamplers[8].ComparisonFunc = ConvertToD3D12ComparisonFunc(desc.compareOp);
            staticSamplers[8].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            staticSamplers[8].MinLOD = desc.minLod;
            staticSamplers[8].MaxLOD = desc.maxLod;
            staticSamplers[8].ShaderRegister = 108;
            staticSamplers[8].RegisterSpace = 0;
            staticSamplers[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
        versionedRootSignatureDesc.Init_1_1(_countof(rootParameters),rootParameters,_countof(staticSamplers),staticSamplers,rootSignatureFlags);

        ID3DBlob* serializedRootSignature;
        D3D12_CHECK(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, &serializedRootSignature, nullptr));

        D3D12_CHECK(
            device->CreateRootSignature(
              0,
			  serializedRootSignature->GetBufferPointer(),
			  serializedRootSignature->GetBufferSize(),
			  IID_PPV_ARGS(&rootSignature)
            )
        );

        D3D12_CHECK(rootSignature->SetName(L"BindlessRootSignature"));

        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};

        allocatorDesc.pDevice = device.Get();
        //ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED: 默认池不零化,ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED: MSAA纹理总是创建Committed资源
        allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAGS(D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED | D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED);

        D3D12_CHECK(D3D12MA::CreateAllocator(&allocatorDesc, &allocator));

        return true;

    }

    void D3D12Device::Exit() 
    {
        WaitIdle();

        delete resourceDescriptorHeap;
        delete samplerDescriptorHeap;

        for (uint32 index = 0; index < (uint32)D3D12CommandQueueType::Count; index++)
        {
            if (commandQueues[(uint32)index])
            {
                delete commandQueues[(uint32)index];
            }
        }
    }
    
    RenderBackend* RenderBackendCreateDirect3D12(const RenderBackendDesc* desc)
    {
        
        D3D12RenderBackend* d3d12Backend = new D3D12RenderBackend();
        if (!d3d12Backend->Init(desc)) 
        {
            delete d3d12Backend;
            return nullptr;
        }
       
        return d3d12Backend;
        
    }

    void RenderBackendDestroyDirect3D12(RenderBackend* backend) 
	{
		D3D12RenderBackend* d3d12Backend = (D3D12RenderBackend*)backend;
		d3d12Backend->Exit();
		delete d3d12Backend;
	}
    
}