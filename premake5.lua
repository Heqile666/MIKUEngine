workspace "MIKUEngine"
    architecture "x64"
    startproject "Sandbox"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"--debug-windows-x64

--这里的是文件夹下的源代码文件
IncludeDir = {}
IncludeDir["GLFW"] = "MIKUEngine/vender/GLFW/include"
IncludeDir["ImGui"] = "MIKUEngine/vender/imgui"
IncludeDir["DX12"] = "MIKUEngine/vender/DX12/include/directx"
IncludeDir["glm"] = "MIKUEngine/vender/glm"

include "MIKUEngine/vender/GLFW" --包含的是GLFW文件夹下的premake5.lua
include "MIKUEngine/vender/imgui" --包含的是ImGui文件夹下的premake5.lua



project "MIKUEngine"
    location "MIKUEngine"
    kind "SharedLib"
    language "C++"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "mikupch.h"
    pchsource "MIKUEngine/src/mikupch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vender/glm/glm/**.hpp",
        "%{prj.name}/vender/glm/glm/**.inl",
    }

    includedirs
    {
        "%{prj.name}/vender/spdlog/include",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.DX12}",
        "%{IncludeDir.glm}",
        
    }

    links{
        --DX12
        "d3dcompiler.lib",
        "D3D12.lib",
        "dxgi.lib",
        --windows
        "GLFW",--link的vs项目
        "opengl32.lib",
        "ImGui"
      
    }
    
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        buildoptions { "/utf-8" }
        
        defines
        {
            "MIKU_BUILD_DLL",
            "MIKU_PLATFORM_WINDOWS"        
        }
        
        postbuildcommands {
            "{MKDIR} %{cfg.targetdir}/../Sandbox/",
            "{COPY} %{cfg.targetdir}/MIKUEngine.dll %{cfg.targetdir}/../Sandbox/"
        }
    filter "configurations:Debug"
        defines "MIKU_DEBUG"
        runtime "Debug"
        optimize "On"

    filter "configurations:Release"
        defines "MIKU_RELEASE"
        runtime "Release"
        optimize "On" 

    filter "configurations:Dist"
        defines "MIKU_DIST"
        runtime "Release"
        optimize "On"

   


project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    buildoptions { "/utf-8" }
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }

    includedirs
    {
        "MIKUEngine/vender/spdlog/include",
        "MIKUEngine/src",
        "%{IncludeDir.glm}",
    }

    links
    {
        "MIKUEngine"
    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"
        
        defines
        {
            "MIKU_PLATFORM_WINDOWS"        
        }
        
            
    filter "configurations:Debug"
        defines "MIKU_DEBUG"
        buildoptions "/MDd"
        optimize "On"

    filter "configurations:Release"
        defines "MIKU_RELEASE"
        buildoptions "/MD"
        optimize "On" 

    filter "configurations:Dist"
        defines "MIKU_DIST"
        buildoptions "/MD"
        optimize "On"
   