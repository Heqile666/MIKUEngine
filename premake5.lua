-- ============================================================
--  MIKUEngine 根构建脚本（对齐 Horizon 风格）
--  - 根脚本只负责 workspace 级配置 + 路径辅助 + 分组 include
--  - 每个工程在自己目录下的 premake5.lua 中定义
--  - imgui 作为独立 StaticLib 工程；glfw 使用预编译库
-- ============================================================

-- ============================================================
--  语法速览（premake 脚本本质就是 Lua 脚本）
--  · 注释：以 -- 开头到行尾
--  · 变量：直接 名字 = 值 即可（无需声明类型）；不加 local 就是“全局变量”，
--          可被后续被 include 进来的子 premake5.lua 共享使用
--  · 字符串拼接用 ..（两个点），例如 "a" .. "/" .. "b" == "a/b"
--  · {} 表示 table（Lua 里既当数组也当字典/哈希表）
--  · %{...} 是 premake 的“Token”——不是 Lua 语法，而是在“生成 .vcxproj 时”
--          才被展开的占位符，如 %{wks.location}=解决方案根目录
-- ============================================================

-- ---------- 路径辅助函数（对齐 Horizon 的 enginepath/thirdpartypath/sourcedirs）----------

-- 全局变量：源码根目录。%{wks.location} 会在生成时展开成实际的解决方案目录
sourcedir = "%{wks.location}/Source"
-- 定义函数 sourcepath(p)：把传入的相对路径 p 拼到 sourcedir 后面并返回
--   function 名字(参数) ... end 是 Lua 的函数定义写法
--   例：sourcepath("MikuEngine/src") 返回 "%{wks.location}/Source/MikuEngine/src"
function sourcepath(p) return sourcedir .. "/" .. p end

-- 同理：第三方库根目录 + 拼接函数
thirdpartydir = "%{wks.location}/ThirdParty"
-- 例：thirdpartypath("imgui") 返回 "%{wks.location}/ThirdParty/imgui"
function thirdpartypath(p) return thirdpartydir .. "/" .. p end

-- 把一个目录下的所有源码一次性加入 files{}（Horizon 同名封装）
-- 参数 dirs 可以是单个字符串，也可以是字符串数组（table）
function sourcedirs(dirs)
    -- type(x) 返回 x 的类型字符串；这里判断：如果传进来不是 table（即传的是单个字符串），
    -- 就把它包成只有一个元素的 table，方便后面统一用 for 遍历
    if type(dirs) ~= "table" then dirs = { dirs } end
    -- ipairs(dirs) 按顺序遍历数组；_ 是“忽略的下标”，dir 是当前元素（目录字符串）
    for _, dir in ipairs(dirs) do
        -- files{} 是 premake 的 API：把匹配到的文件加入当前工程
        -- ** 是递归通配符（匹配任意层子目录），例如 dir.."/**.cpp" = 该目录下所有 .cpp
        files {
            dir .. "/**.h",
            dir .. "/**.hpp",
            dir .. "/**.inl",
            dir .. "/**.c",
            dir .. "/**.cpp",
        }
    end
end

-- 输出目录名，形如 Debug-windows-x86_64（%{cfg.*} 也是生成期 Token；部分场景仍会用到）
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- GLFW 预编译库根目录（调用上面的函数得到完整路径字符串）
GLFWDir = thirdpartypath("glfw/glfw-3.4.bin.WIN64")

-- ---------- 第三方 include 目录集中管理 ----------
-- IncludeDir 是一个 table（当字典用）：键是库名字符串，值是该库的头文件目录
-- 好处：include 目录只在这里写一次，各子工程用 "%{IncludeDir.GLFW}" 引用即可
-- 语法：IncludeDir["键"] = 值   等价于   IncludeDir.键 = 值
IncludeDir = {}
IncludeDir["GLFW"]   = GLFWDir .. "/include"              -- 预编译 GLFW 的头目录
IncludeDir["ImGui"]  = thirdpartypath("imgui")            -- imgui 根目录（含 imgui.h）
IncludeDir["glm"]    = thirdpartypath("glm")              -- glm 为纯头文件库
IncludeDir["spdlog"] = thirdpartypath("spdlog/include")   -- spdlog 头目录
IncludeDir["DX12"]   = thirdpartypath("DX12/include/directx") -- DirectX-Headers

-- ---------- 预编译库文件（.lib）集中管理 ----------
-- 与 IncludeDir 同理，但值是要链接的 .lib 完整路径；各工程用 "%{Lib.GLFW}" 引用
Lib = {}
Lib["GLFW"] = GLFWDir .. "/lib-vc2022/glfw3.lib" -- 选 /MD(动态运行时)版本，匹配下方 staticruntime "Off"

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
