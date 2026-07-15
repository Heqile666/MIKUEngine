-- ============================================================
--  MikuEditorLauncher —— 可执行程序（对齐 Horizon 的 HorizonEditorLauncher）
-- ============================================================
project "MikuEditorLauncher"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    location "%{wks.location}/Build/%{prj.name}"
    targetdir "%{wks.location}/Build/Bin/%{cfg.buildcfg}"
    objdir    "%{wks.location}/Build/Obj/%{cfg.buildcfg}/%{prj.name}"
    debugdir  "%{cfg.targetdir}"

    buildoptions { "/utf-8" }

    files {
        "**.h",
        "**.cpp",
    }

    includedirs {
        sourcepath("MikuEditor/Source"),
        sourcepath("MikuEngine/src"),
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.DX12}",
        "%{IncludeDir.glm}",
    }

    -- exe 需显式链接所有外部库
    links {
        "MikuEngine",
        "MikuEditor",
        "imgui",
        "%{Lib.GLFW}",
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
