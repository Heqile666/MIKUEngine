-- ============================================================
--  MikuEditor —— 编辑器静态库壳（对齐 Horizon 的 HorizonEditor）
-- ============================================================
project "MikuEditor"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "Off"
    location "%{wks.location}/Build/%{prj.name}"
    targetdir "%{wks.location}/Build/Bin/%{cfg.buildcfg}"
    objdir    "%{wks.location}/Build/Obj/%{cfg.buildcfg}/%{prj.name}"

    buildoptions { "/utf-8" }

    sourcedirs { "Source" }

    includedirs {
        "Source",
        sourcepath("MikuEngine/src"),
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.DX12}",
        "%{IncludeDir.glm}",
    }

    links {
        "MikuEngine",
        "imgui",
    }

    filter "system:windows"
        systemversion "latest"
        defines {
            "MIKU_PLATFORM_WINDOWS",
        }
    filter {}
