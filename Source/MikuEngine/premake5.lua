-- ============================================================
--  MikuEngine —— 引擎静态库
--  imgui 由独立工程编译（含 backend）；glfw 使用预编译库
-- ============================================================
project "MikuEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    location "%{wks.location}/Build/%{prj.name}"
    targetdir "%{wks.location}/Build/Bin/%{cfg.buildcfg}"
    objdir    "%{wks.location}/Build/Obj/%{cfg.buildcfg}/%{prj.name}"

    buildoptions { "/utf-8" }
    linkoptions  { "/ignore:4006" }

    pchheader "mikupch.h"
    pchsource "Source/mikupch.cpp"

    -- 引擎自身源码
    sourcedirs { "Source" }

    -- Platform/ 与旧 ImGui/(ImGuiLayer) 是之前项目的临时代码：
    -- 从工程中排除（文件保留在磁盘供后续搬运参考），改由引擎内 GlfwWindow + RenderContext 胶水替代
    removefiles {
        "Source/Platform/**",
        "Source/ImGui/**",
    }

    includedirs {
        "Source",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.DX12}",
        "%{IncludeDir.glm}",
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
    }

    links {
        "imgui",        -- imgui 独立工程（含 backend）
        "%{Lib.GLFW}",  -- GLFW 预编译库
        "d3dcompiler.lib",
        "D3D12.lib",
        "dxgi.lib",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "MIKU_PLATFORM_WINDOWS",
        }
    filter {}
