# Horizon RenderGraph 功能详解（从零理解版）

> 目标：即使你完全没接触过 RenderGraph，读完这份也能理解它"是什么、为什么要有、怎么运转"。
> 对象：Horizon 引擎 `Source\HorizonEngine\Source\Rendering\RenderGraph\`（18 个文件，约 2100 行）。
> 说明：本文所有机制都对照过源码；凡是"设计上有、但当前代码里没接线/是空实现/被注释"的，都会**明确标注真实状态**，避免你被"看起来支持"误导。

---

## 0. 一句话理解：RenderGraph 是什么

**RenderGraph（渲染图 / Frame Graph）是一个"帧级别的任务调度器"。**

你不再手写"先给纹理加屏障、开 RenderPass、画、再改回屏障"这些琐碎又易错的底层操作；而是**声明式**地告诉它：

> "我这个 Pass 要**读** A 纹理、**写** B 纹理，执行逻辑是这段 Lambda。"

RenderGraph 收集完整帧里所有 Pass 的读写声明后，自动帮你：
1. 分配临时 GPU 资源（并跨帧复用）；
2. 计算并插入资源状态屏障（Barrier）；
3. 按顺序执行每个 Pass 的绘制/计算 Lambda；
4. 帧末把资源恢复到该有的状态、回收资源。

### 对比：手动管理 vs RenderGraph

```
【传统手动方式】——每一步屏障都要自己写，写错就崩/画面错
  Barrier(tex, SRV → RenderTarget)
  BeginRenderPass(...)
  Draw(...)
  EndRenderPass()
  Barrier(tex, RenderTarget → SRV)

【RenderGraph 声明式】——只描述意图，屏障/RenderPass 自动生成
  renderGraph.AddPass("GBuffer", Graphics, [&](RenderGraphBuilder& builder) {
      builder.SetRenderTargetBinding(0, gbuffer, Load::Clear, Store::Store); // 声明"我要写它"
      return [=](CommandList& cmd, const ResourceRegistry& reg) {
          cmd.Draw(...);   // 只写真正的绘制逻辑
      };
  });
  // Barrier / BeginRenderPass / EndRenderPass 由 RenderGraph 在 Execute() 时自动补齐
```

**核心价值**：把"资源状态管理"这件全局性、极易出错的事，从人脑转移给了系统。

---

## 1. 整体架构与它在引擎中的位置

```
应用层 (Editor / Game)
   │ 调用 SceneRenderer::RenderScene()
   ▼
Renderer 框架层  (RasterizationRenderer / PathTracingRenderer)
   │ 把每种渲染技术 AddPass 注册进图
   ▼
★ RenderGraph 层 ★   ← 本文主角
   │ 声明 Pass+资源依赖 → (可选)编译剔除 → 执行:分配资源/插屏障/跑Lambda
   ▼ 生成 RenderBackendCommandList 命令
RHI 层 (RenderBackend / DX12)
   │ CreateBuffer / CreateTexture / Barriers / BeginRenderPass / Draw / SubmitCommandLists
   ▼
GPU
```

RenderGraph 上承 Renderer（渲染技术在这里被组织成 Pass），下接 RHI（最终翻译成 DX12 命令）。

---

## 2. 使用者视角：一个 Pass 长什么样

这是理解一切的起点。用户只跟 `AddPass` 打交道：

```cpp
renderGraph.AddPass(
    "Lighting",                            // ① Pass 名字（调试/Profiler 用）
    RenderGraphPassFlags::Compute,         // ② Pass 类型
    [&](RenderGraphBuilder& builder)       // ③ setup lambda —— 声明阶段
    {
        builder.SetBindlessResourceSRV(0, gbufferTexture);   // 输入
        builder.SetBindlessResourceSRV(1, depthTexture);     // 输入
        builder.SetBindlessResourceUAV(2, outputTexture, 0); // 输出
        builder.SetShaderConstantValue(3, lightCount);       // 标量参数

        return [=](RenderBackendCommandList& cmd,            // ④ execute lambda —— 执行阶段
                   const RenderGraphResourceRegistry& reg)
        {
            cmd.Dispatch(lightingShader, reg.GetPushConstantValues(), gx, gy, 1);
        };
    });
```

**两段 Lambda 是关键，且时机完全不同**（已对照 `RenderGraph.h:203-212` 核实）：

| Lambda | 何时运行 | 作用 |
|--------|---------|------|
| **setup**（③，参数是 Builder） | **在 `AddPass()` 调用内立即同步执行** | 声明这个 Pass 读写哪些资源、绑定哪些参数 |
| **execute**（④，setup 的返回值） | **延迟到 `Execute()` 时才调用** | 真正下达 Draw/Dispatch 命令 |

源码实证：
```cpp
// RenderGraph.h:208-209
const auto& execute = setup(builder);          // setup 立即执行
pass->SetExecuteCallback(std::move(execute));  // execute 只是被保存，稍后才调
```

> 直观理解：setup 是"填申报表"（我要用哪些料），execute 是"真正开工"。系统在两者之间插入了它的调度魔法。

---

## 3. Pass 的类型（RenderGraphPassFlags）

```cpp
enum class RenderGraphPassFlags : uint32 {
    Copy           = 1<<0,  // 纯拷贝
    Compute        = 1<<1,  // 计算着色器
    AsyncCompute   = 1<<2,  // 异步计算队列
    Graphics       = 1<<3,  // 图形绘制（会自动包裹 Begin/EndRenderPass）
    MeshShading    = 1<<4,  // Mesh Shader 绘制
    RayTracing     = 1<<5,  // 光线追踪
    NeverGetCulled = 1<<6,  // 永不被剔除（引用计数设为无穷大）
    SkipRenderPass = 1<<7,  // 是 Graphics 但不自动包 RenderPass
    Readback       = Copy | NeverGetCulled,
};
```

其中 `Graphics` 会让 RenderGraph 在执行该 Pass 前后自动调用 `BeginRenderPass / EndRenderPass`；`Compute` 则不会。

---

## 4. 句柄系统：index + version

RenderGraph 里的资源不是直接用 RHI 指针，而是用**轻量句柄**：

```cpp
class RenderGraphHandleBase {
    uint32 index;    // 在资源数组里的下标
    uint32 version;  // 版本号
};
```
（`RenderGraphHandles.h:67-71`）

- **index**：资源存放位置的下标。
- **version**：用于区分"同一资源被写入后产生的新逻辑版本"。`operator==` 会同时比较 index 和 version（`:45-48`）。
- **CreateNewVersion(old)**：保持 index、version+1，返回新句柄（`:81-85`）。理论用途是"写操作产生资源的新版本"，读旧版本 / 读新版本可被区分，从而正确建依赖（这是 UE RDG 同款思路）。

> ⚠️ **真实状态**：`CreateNewVersion` 在当前工程里**没有任何调用点**，创建句柄一律用 `version = 0`。所以 **version 机制目前是"已定义但尚未接线"的预留能力**，并没有真正参与依赖追踪。理解概念即可，别以为它正在工作。

类型化别名：
```cpp
using RenderGraphTextureHandle = RenderGraphHandle<RenderGraphTexture>;
using RenderGraphBufferHandle  = RenderGraphHandle<RenderGraphBuffer>;
```

---

## 5. 资源系统：逻辑资源与状态追踪

### 5.1 资源基类字段（`RenderGraphResources.h:89-98`）

```cpp
class RenderGraphResource : public RenderGraphNode {
    bool imported;        // 外部导入（不由图内分配）
    bool exported;        // 需导出给图外持有
    bool transient;       // 瞬态（仅图内生命周期）
    bool usedByAsyncComputePass;

    RenderBackendResourceState initialState;       // 进入图时的状态
    RenderBackendResourceState finalState;         // 图结束时应恢复到的状态
    RenderBackendResourceState intermediateState;  // ★执行期实时追踪的"当前状态"

    RenderGraphPass* firstPass;   // 第一个用它的 Pass
    RenderGraphPass* lastPass;    // 最后一个用它的 Pass
};
```

**三个 state 字段是屏障自动化的核心**：
- `initialState` / `finalState`：帧首/帧末的目标状态。
- `intermediateState`：随每个 Pass 推进而更新的"当前实时状态"。屏障就是靠比较"当前 intermediateState"与"下一个 Pass 需要的状态"来决定要不要插入（见第 8 章）。

> 真实状态：`transient / usedByAsyncComputePass / firstPass / lastPass` 目前主要是字段声明，尚未被密集使用，可视为"生命周期/异步元数据（部分预留）"。真正驱动执行的是三个 state 字段。

### 5.2 Texture / Buffer

- `RenderGraphTexture` 封装 `RenderGraphTextureDescription`（= `RenderBackendTextureDesc`）+ 指向底层持久纹理的指针。
- `RenderGraphBuffer` 封装 `RenderBackendBufferDescription` + 底层 Buffer 指针。

描述类型直接复用 RHI 类型（`using RenderGraphTextureDescription = RenderBackendTextureDesc;`），**图层和 RHI 层描述一致，无需转换**。

---

## 6. Builder 接口：如何声明依赖

`RenderGraphBuilder` 是 setup lambda 里传进来的对象，所有"读/写/绑定"声明都通过它完成（`RenderGraphBuilder.h/.cpp`）。

### 6.1 读写声明

| 方法 | 加入 | referenceCount 递增对象 | 备注 |
|------|------|------------------------|------|
| `ReadTexture(h, state)` | `pass->inputs` | **资源** referenceCount++ | 记录 textureState(initial==final) |
| `WriteTexture(h, state)` | `pass->outputs` | **pass** referenceCount++ | |
| `WriteTexture(h, init, final)` | `pass->outputs` | pass++ | 可指定不同的 finalState |
| `ReadWriteTexture(h, state)` | inputs **和** outputs | pass++ | |
| `ReadBuffer(h, state)` | `pass->inputs` | pass++ | |
| `WriteBuffer(h, state)` | `pass->outputs` | pass++ | |

> 值得注意的真实细节：**纹理 Read 递增的是"资源"的引用计数，而 Write / 所有 Buffer 递增的是"pass"的引用计数**——这是源码里真实存在的不对称写法，剔除算法（第 9 章）就是基于这些计数运行的。

> ⚠️ 空桩函数（当前无实际逻辑）：`CreateTransientTexture`、`CreateTransientBuffer`、`ReadWriteBuffer` 目前只是返回默认/原 handle，**没有建立任何依赖**。用到时需自己补实现。

### 6.2 Bindless 资源绑定：Tracked vs Untracked（重要区别）

RenderGraph 走的是 **Bindless（无绑定）** 模型：资源不占固定寄存器槽，而是把"描述符索引"作为整数塞进 PushConstants，Shader 用索引去全局描述符堆里取资源。绑定分两类：

**① Untracked（参数是 RHI 句柄 `RenderBackendBufferHandle`）**
```cpp
builder.SetBindlessResourceSRV(slot, rhiBufferHandle);
```
- **立即**向 RHI 查询 descriptorIndex，把解析好的整数存进 binding。
- **不进 inputs/outputs、不改 referenceCount、不追踪状态**。
- 适合"图外自己管好生命周期与状态"的资源（如常驻的全局 Buffer）。调用者自己负责其正确性。

**② Tracked（参数是 RenderGraph 句柄 `RenderGraphTextureHandle` / `RenderGraphBufferHandle`）**
```cpp
builder.SetBindlessResourceSRV(slot, rgTextureHandle);
builder.SetBindlessResourceUAV(slot, rgTextureHandle, mipLevel);
```
- 记录进依赖图，**参与状态追踪与自动屏障**，索引在执行期才解析。

**③ 标量常量**
```cpp
builder.SetShaderConstantValue(slot, intValue / uintValue / floatValue);
```
把标量直接写进对应 PushConstant slot。

### 6.3 RenderTarget / DepthStencil 绑定（Graphics Pass 专用）

```cpp
builder.SetRenderTargetBinding(slot, texHandle, loadOp, storeOp, mipLevel=0);
builder.SetDepthStencilBinding(texHandle, depthLoad, depthStore, stencilLoad, stencilStore, accessType);
```
- `loadOp`：Clear / Load / DontCare；`storeOp`：Store / DontCare。
- **`SetRenderTargetBinding` 内部会自动帮你 `WriteTexture(handle, RenderTarget)`** —— 你不用再单独声明写依赖。

---

## 7. PushConstants 的组装（ResourceRegistry）

execute lambda 里拿到的是 `RenderGraphResourceRegistry`，它负责把"句柄"翻译成 RHI 真实句柄 / Bindless 索引，并组装最终的 PushConstants：

```cpp
RenderBackendPushConstantValues GetPushConstantValues() {
    for (i = 0; i < SlotCount; i++) {
        switch (pass->shaderConstantsBindings[i].type) {
            case UntrackedResource: values[i] = binding.descriptorIndex; break;              // 已解析好的索引
            case BufferSRV:  values[i] = GetBufferSRVBindlessResourceDescriptorIndex(h);  break;
            case BufferUAV:  values[i] = GetBufferUAVBindlessResourceDescriptorIndex(h);  break;
            case TextureSRV: values[i] = GetTextureSRVBindlessResourceDescriptorIndex(h); break;
            case TextureUAV: values[i] = GetTextureUAVBindlessResourceDescriptorIndex(h,0); break;
            case ScalarInt:   values[i] = binding.scalarTypeInt;   break;
            case ScalarFloat: values[i] = binding.scalarTypeFloat; break;
        }
    }
}
```

也就是说：**Builder 阶段登记"每个 slot 绑什么"，Registry 在执行期把它们统一翻译成一串整数**，作为 PushConstants 传给 Draw/Dispatch。这就是 Bindless 渲染的核心串联点。

---

## 8. 执行流程与"屏障自动插入"（最核心）

`RenderGraph::Execute(commandList)` 的真实步骤（对照 `RenderGraph.cpp`）：

```
Execute(commandList)
│
├─ ①(Compile 被注释掉了，见第 9 章，当前不剔除)
│
├─ ② 为每个"非导入且尚无内部资源"的 Texture/Buffer 从 ResourcePool 分配 GPU 资源
│      if (!texture->IsImported() && !hasInternalTexture)
│          texture->internalTexture = resourcePool->AllocateTexture(desc)
│
├─ ③ 执行 Buffer 上传任务 (ExecuteBufferUploadJobs)
│      CopyBuffer(staging → target) + Barrier(CopyDst → ShaderResource)
│
├─ ④ 逐 Pass 执行：
│    for (pass : passes)
│        BeginDebugLabel + GPU Profiler 开始
│        ├─ 【Texture 屏障】对该 Pass 声明的每个 textureState:
│        │     若 资源.intermediateState ≠ 本 Pass 需要的 initialState
│        │        → 生成一条 Barrier(intermediateState → initialState)
│        │        → 更新 资源.intermediateState = 该 Pass 结束后的状态
│        │     （UAV→UAV 等特殊情况按需生成 UAV Barrier）
│        ├─ 【Buffer 屏障】同理，比较并更新 buffer 的 intermediateState
│        ├─ commandList.Barriers(pass->barriers)   ← 把这一批屏障一次性下达
│        ├─ 若是 Graphics Pass: 组装 RenderPassInfo(RTV/DSV/RenderArea) → BeginRenderPass
│        ├─ pass->Execute(commandList, resourceRegistry)  ← 你的 execute lambda 在这里跑
│        ├─ 若是 Graphics Pass: EndRenderPass
│        └─ GPU Profiler 结束 + EndDebugLabel
│
├─ ⑤ 帧末：把资源从 intermediateState 恢复到 finalState
│      if (资源.intermediateState ≠ 资源.finalState) → Barrier
│
└─ ⑥ 处理 Export（需要跨帧保留的资源，把内部资源写回外部指针）
```

**屏障自动化的本质**（这是 RenderGraph 最大价值）：
- 每个资源身上带一个"当前实时状态" `intermediateState`。
- 每进入一个 Pass，系统比较"资源当前状态"与"这个 Pass 需要它处于的状态"；**不一致就自动插一条状态转换 Barrier**，并把 `intermediateState` 更新为新状态。
- 你从头到尾没写过一条 Barrier —— 全是系统根据你的 read/write 声明推出来的。

---

## 9. 编译阶段（死代码剔除）——已实现但当前关闭

`Compile()` 实现了经典的"引用计数式死代码剔除"（`RenderGraph.cpp:294-326`）：

```
1. 把所有 referenceCount == 0 的节点压入待剔除栈
2. 弹栈：对被剔除节点的每个 input，其 referenceCount--；若归零则也入栈
3. 循环直到栈空；最终 referenceCount==0 的 Pass/资源视为"死代码"，执行时跳过
```
配合 `NeverGetCulled`（把引用计数设为无穷大 `(uint32)-1`）可让某 Pass 永不被剔除。

> ⚠️ **真实状态（重要）**：`Compile()` 在 `Execute()` 开头**被整段注释掉**：
> ```cpp
> // RenderGraph.cpp:357-360
> //if (!Compile()) {
> //    return;
> //}
> ```
> 全工程没有任何地方真正调用 `Compile()`，与之配套的 `IsCulled()` 判断也大多被注释。
> **所以当前所有 Pass 都会无条件执行，死代码剔除功能未生效。** 这是"设计具备、暂未启用"的能力。

---

## 10. 资源池与生命周期（性能关键）

### 10.1 复用策略（`RenderGraphResourcePool`）

```cpp
PersistentTexture* AllocateTexture(desc, name) {
    for (tex : allocatedTextures)
        if (!tex->active && tex->desc == desc)   // 描述匹配 + 当前不活跃 → 直接复用！
            { tex->active = true; return tex; }
    // 没有可复用的 → 才真正 CreateTexture 新建 GPU 资源
    handle = backend->CreateTexture(&desc, ...);
    return new PersistentTexture(name, backend, desc, handle);
}

void ReleaseTexture(tex) { tex->active = false; }  // 只标记不活跃，不销毁 GPU 资源

void Tick() { for (tex) tex->active = false; }      // 每帧开头重置所有 active 标记
```

**核心设计（已核实属实）**：
- 临时资源**从不在帧内销毁**，只在"不用了"时标记 `active = false`。
- 下一帧若有**相同描述**的需求，直接复用同一块 GPU 资源，避免反复创建/销毁的开销。
- 判断可复用的条件是：`!active && desc 完全匹配`。

这就是为什么临时 RT / 中间纹理数量再多，GPU 显存占用也能收敛——本质是一个按描述匹配的对象池。

### 10.2 持久化资源

- `RenderGraphPersistentTexture`：持有真实 `RenderBackendTextureHandle`，并按需缓存 RTV（`FindOrCreateRenderTargetView(mipLevel)`：查已有、没有才创建）。
- `RenderGraphPersistentBuffer`：持有真实 `RenderBackendBufferHandle`。

### 10.3 Staging Buffer 上传池

- 设计上有三重缓冲的上传 Buffer 池（`BufferUploader bufferUploader[3]`）用于跨帧轮转上传数据。
- ⚠️ **真实状态**：`freeStagingBuffers` 目前只有"取"没有"还"——分配出去的 staging buffer 没有被回收进空闲列表，**回收闭环尚未接线**。理解其意图即可，实际使用需注意这个未完成点。

---

## 11. Blackboard：跨 Pass 数据共享

Pass 之间要传递句柄（比如 GBuffer Pass 产出的几张纹理，要给 Lighting Pass 用），用 `RenderGraphBlackboard`——一个**以 C++ 类型为 key 的类型安全键值存储**：

```cpp
// 定义共享结构
struct GBufferData { RenderGraphTextureHandle albedo, normal, depth; };

// GBuffer Pass 写入
auto& data = renderGraph.blackboard.Create<GBufferData>();
data.albedo = builder.WriteTexture(...);

// Lighting Pass 读取
auto& data = renderGraph.blackboard.Get<GBufferData>();
builder.SetBindlessResourceSRV(0, data.albedo);
```

原理：每种类型通过一个 `static` 计数器分配到唯一 index，`Create<T>()`/`Get<T>()` 靠该 index 做 O(1) 存取；生命周期绑定到帧内 Arena。每种类型只 `Create()` 一次。

---

## 12. 完整一帧数据流（把所有环节串起来）

```
SceneRenderer::RenderScene(renderGraph, scene, view)
│
├─ RasterizationRenderer::Render(...)   ← 这里全是 AddPass（setup 立即执行，建依赖图）
│    AddPass("GBuffer",          Graphics, ...)
│    AddPass("ShadowMap",        Graphics, ...)
│    AddPass("DepthPyramid",     Compute,  ...)
│    AddPass("SSAO",             Compute,  ...)
│    AddPass("DeferredShading",  Compute,  ...)
│    AddPass("SSR/Atmosphere/...",Compute, ...)
│    AddPass("TSS/Bloom/Tonemap",Compute,  ...)
│    AddPass("FinalComposition", Compute,  ...)
│
└─ renderGraph.Execute(commandList)     ← execute lambda 在这里才真正跑
     ├─ 从 ResourcePool 分配/复用临时 GPU 资源
     ├─ 上传 Buffer 数据
     ├─ 逐 Pass：自动插 Barrier → (Graphics)BeginRenderPass → 跑 Lambda → EndRenderPass
     └─ 恢复资源最终状态 / 处理 Export
        │
        ▼
renderBackend->SubmitCommandLists(...)   → DX12 ExecuteCommandLists
swapChain->Present()
resourcePool->Tick()   (所有资源 active 归 false，准备下一帧复用)
device->Tick()         (回收已完成的 CommandAllocator)
```

---

## 13. 关键要点回顾（记住这几条就够用）

1. **声明式**：你只 `AddPass` 声明"读什么、写什么、执行什么"，不碰底层屏障。
2. **两段 Lambda**：setup 在 AddPass 时**立即**执行（建依赖）；execute **延迟**到 Execute 时执行（下命令）。
3. **屏障全自动**：靠每个资源的 `intermediateState` 与各 Pass 需要的状态做比较，不一致就自动插 Barrier。
4. **资源池复用**：临时资源从不在帧内销毁，只按 `active` 标记 + `desc` 匹配跨帧复用。
5. **Bindless + PushConstants**：Builder 登记 slot 绑定，Registry 执行期翻译成整数索引数组传给 Shader。
6. **Blackboard**：以类型为 key，在 Pass 间安全传递资源句柄。

### 当前版本"设计有、但尚未生效/未接线"的点（务必知道）
| 功能 | 真实状态 |
|------|---------|
| `Compile()` 死代码剔除 | 已实现，但在 Execute 中**被注释**，当前所有 Pass 都执行 |
| 句柄 `version` / `CreateNewVersion` | 已定义，但**无调用点**，version 恒为 0，未参与依赖追踪 |
| `CreateTransientTexture/Buffer`、`ReadWriteBuffer` | **空桩函数**，不建立任何依赖 |
| Staging Buffer 三重缓冲回收 | 只分配不回收，**回收闭环未接线** |
| `transient / usedByAsyncComputePass / firstPass / lastPass` | 主要是字段声明，未被密集使用（部分预留） |

> 这些不影响你理解 RenderGraph 的设计思想，但如果你要基于这套代码做移植或扩展，这几处是需要补齐/注意的地方。
