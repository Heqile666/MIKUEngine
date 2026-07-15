-- ============================================================
--  ThirdParty 构建脚本（由 superproject 跟踪，不放进 imgui 子模块内部）
--  目前定义 imgui 独立 StaticLib 工程（对齐 Horizon）
-- ============================================================
project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    location "%{wks.location}/Build/%{prj.name}"
    targetdir "%{wks.location}/Build/Bin/%{cfg.buildcfg}"
    objdir    "%{wks.location}/Build/Obj/%{cfg.buildcfg}/%{prj.name}"

    files {
        -- 核心
        "imgui/imconfig.h",
        "imgui/imgui.h",
        "imgui/imgui.cpp",
        "imgui/imgui_draw.cpp",
        "imgui/imgui_internal.h",
        "imgui/imgui_tables.cpp",
        "imgui/imgui_widgets.cpp",
        "imgui/imgui_demo.cpp",
        "imgui/imstb_rectpack.h",
        "imgui/imstb_textedit.h",
        "imgui/imstb_truetype.h",
        -- backend：DX12 + GLFW
        "imgui/backends/imgui_impl_dx12.h",
        "imgui/backends/imgui_impl_dx12.cpp",
        "imgui/backends/imgui_impl_glfw.h",
        "imgui/backends/imgui_impl_glfw.cpp",
    }

    includedirs {
        "imgui",               -- imgui 根目录（imgui.h）
        "%{IncludeDir.GLFW}",  -- imgui_impl_glfw.cpp 需要 <GLFW/glfw3.h>
        "%{IncludeDir.DX12}",  -- imgui_impl_dx12.cpp 需要 d3d12 相关头
    }

    links {
        "%{Lib.GLFW}",         -- backend 依赖 GLFW
    }

    filter "system:windows"
        systemversion "latest"
    filter {}
