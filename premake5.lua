-- ============================================================
--  MIKUEngine 根构建脚本（对齐 Horizon 风格）
--  - 根脚本只负责 workspace 级配置 + 路径辅助 + 分组 include
--  - 每个工程在自己目录下的 premake5.lua 中定义
--  - imgui 作为独立 StaticLib 工程；glfw 使用预编译库
-- ============================================================

-- ---------- 路径辅助函数（对齐 Horizon 的 enginepath/thirdpartypath/sourcedirs）----------
sourcedir = "%{wks.location}/Source"
function sourcepath(p) return sourcedir .. "/" .. p end

thirdpartydir = "%{wks.location}/ThirdParty"
function thirdpartypath(p) return thirdpartydir .. "/" .. p end

-- 把一个目录下的所有源码一次性加入 files{}（Horizon 同名封装）
function sourcedirs(dirs)
    if type(dirs) ~= "table" then dirs = { dirs } end
    for _, dir in ipairs(dirs) do
        files {
            dir .. "/**.h",
            dir .. "/**.hpp",
            dir .. "/**.inl",
            dir .. "/**.c",
            dir .. "/**.cpp",
        }
    end
end

-- 输出目录形如 Debug-windows-x86_64（部分场景仍会用到）
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- GLFW 预编译库根目录
GLFWDir = thirdpartypath("glfw/glfw-3.4.bin.WIN64")

-- ---------- 第三方 include 目录集中管理 ----------
IncludeDir = {}
IncludeDir["GLFW"]   = GLFWDir .. "/include"
IncludeDir["ImGui"]  = thirdpartypath("imgui")
IncludeDir["glm"]    = thirdpartypath("glm")
IncludeDir["spdlog"] = thirdpartypath("spdlog/include")
IncludeDir["DX12"]   = thirdpartypath("DX12/include/directx")

-- ---------- 预编译库文件集中管理 ----------
Lib = {}
Lib["GLFW"] = GLFWDir .. "/lib-vc2022/glfw3.lib" -- /MD 版本，匹配 staticruntime Off

-- ============================================================
workspace "MIKUEngine"
    architecture "x64"
    startproject "MikuEditorLauncher"
    configurations { "Debug", "Release", "Dist" }
    flags { "MultiProcessorCompile" }
    staticruntime "Off" -- 使用动态 CRT(/MD)，与 GLFW 预编译库保持一致

filter "system:windows"
    systemversion "latest"

filter "configurations:Debug"
    defines "MIKU_DEBUG"
    runtime "Debug"
    symbols "on"
    optimize "off"

filter "configurations:Release"
    defines "MIKU_RELEASE"
    runtime "Release"
    optimize "on"

filter "configurations:Dist"
    defines "MIKU_DIST"
    runtime "Release"
    optimize "on"

filter {} -- 复位 filter

-- ============================================================
--  工程分组（用 group 组织解决方案资源管理器；不产生真实目录）
-- ============================================================
group "Engine"
    include "Source/MikuEngine"
group ""

group "Editor"
    include "Source/MikuEditor"
group ""

group "App"
    include "Source/MikuEditorLauncher"
group ""

group "ThirdParty"
    include "ThirdParty" -- 加载 ThirdParty/premake5.lua（superproject 跟踪），定义 imgui 独立工程
group ""
