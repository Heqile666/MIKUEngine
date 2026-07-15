# MIKUEngine

一个学习型渲染引擎，目录结构与构建方式对齐 [Horizon](https://github.com/HorizonEngine) 风格：
`Engine + Editor + Launcher` 三段式，第三方库集中于 `ThirdParty/`，使用 [premake5](https://premake.github.io/) 生成 Visual Studio 工程。

---

## 一、环境要求

- Windows 10/11 x64
- Visual Studio 2022（含 **Desktop development with C++** / MSVC v143 工具集、Windows SDK）
- Git（含 submodule 支持）
- DirectX 12 兼容显卡

---

## 二、拉取工程（含子模块）

`imgui / glm / spdlog / DX12` 是 git submodule，克隆时必须带 `--recurse-submodules`：

```bash
git clone --recurse-submodules <repo-url> MIKUEngine
```

如果已经普通克隆过，可补拉子模块：

```bash
git submodule update --init --recursive
```

> GLFW 使用仓库内**预编译库** `ThirdParty/glfw/glfw-3.4.bin.WIN64`（已随仓库提交），无需额外获取。

---

## 三、生成工程

在项目根目录执行：

```bat
GenerateProjects.bat
```

它会调用 `ThirdParty/premake/premake5.exe vs2022`，在 `Build/` 下生成 `.vcxproj`，并在根目录生成 `MIKUEngine.sln`。

> 新增/删除源码文件，或修改任意 `premake5.lua` 后，需重新运行该脚本。

---

## 四、编译与运行

1. 用 VS2022 打开根目录的 `MIKUEngine.sln`。
2. 启动项目默认是 **MikuEditorLauncher**。
3. 选择配置 `Debug|x64`，编译运行。

产物输出在 `Build/Bin/<Config>/`，可执行文件为 `MikuEditorLauncher.exe`。

### 命令行编译（可选）

```bat
msbuild MIKUEngine.sln /t:Build /p:Configuration=Debug /p:Platform=x64
```

> ⚠️ 若本机同时装有多个 MSVC 工具集（如 14.38 与 14.44），命令行构建可能出现
> `error C2338: STL1001: Unexpected compiler version`（编译器与 STL 头版本不一致）。
> 解决：VS IDE 内构建通常自动使用一致工具集；或命令行显式指定，例如
> `... /p:VCToolsVersion=14.38.33130`；或在 VS Installer 修复/统一 v143 工具集。

---

## 五、项目结构

```
MIKUEngine/
├── premake5.lua                 # 根构建脚本（workspace 配置 + 分组 include）
├── GenerateProjects.bat         # 生成 VS 工程
├── MIKUEngine.sln               # premake 生成
├── Build/                       # 生成的工程与产物（Bin/ 可执行与库，Obj/ 中间文件）
├── Source/
│   ├── MikuEngine/              # 引擎（StaticLib）
│   │   └── src/Miku/{Foundation, Engine, Input, Rendering}
│   ├── MikuEditor/              # 编辑器（StaticLib 壳）
│   └── MikuEditorLauncher/      # 可执行入口（ConsoleApp）
└── ThirdParty/
    ├── imgui/  glm/  spdlog/  DX12/   # git submodule
    ├── glfw/glfw-3.4.bin.WIN64/       # 预编译库（随仓库提交）
    └── premake/                       # premake 工具
```

解决方案分组：`Engine/MikuEngine`、`Editor/MikuEditor`、`App/MikuEditorLauncher`、`ThirdParty/imgui`。

启动链路：`MikuEditorLauncher.main() → MikuEditorMain() → MikuEditorApp(Application)::Run()`。

---

## 六、渲染后端说明

当前可执行为**可运行空壳**：打开 GLFW 窗口并跑主循环。DX12 RHI（`Miku/Rendering/RenderBackend`）仍在开发中，
其接入代码位于 `Source/MikuEngine/src/Miku/Engine/RenderContext.cpp`，由编译开关
`MIKU_ENABLE_RENDER_BACKEND`（默认 `0`）控制。RHI 完善后置为 `1` 即可启用真正的清屏/present。

---

更详细的架构与各 premake 文件职责见 [`项目结构整理说明.md`](项目结构整理说明.md)。
