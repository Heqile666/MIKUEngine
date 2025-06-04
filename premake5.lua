workspace "MIKUEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "MIKUEngine/vender/GLFW/include"

include "MIKUEngine/vender/GLFW"


project "MIKUEngine"
    location "MIKUEngine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "mikupch.h"
    pchsource "MIKUEngine/src/mikupch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }

    includedirs
    {
        "%{prj.name}/vender/spdlog/include",
        "%{prj.name}/src",
        "%{IncludeDir.GLFW}"
    }

    links{
        "GLFW",
        "opengl32.lib"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0"
        buildoptions { "/utf-8" }
        
        defines
        {
            "MIKU_BUILD_DLL",
            "MIKU_PLATFORM_WINDOWS"        
        }
        
        postbuildcommands
        {
            ("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" ..outputdir.. "/Sandbox")
        }
        
    filter "configurations:Debug"
        defines "MIKU_DEBUG"
        optimize "On"

    filter "configurations:Release"
        defines "MIKU_RELEASE"
        optimize "On" 

    filter "configurations:Dist"
        defines "MIKU_DIST"
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
        "MIKUEngine/src"
    }

    links
    {
        "MIKUEngine"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0"
        
        defines
        {
            "MIKU_PLATFORM_WINDOWS"        
        }
        
            
    filter "configurations:Debug"
        defines "MIKU_DEBUG"
        optimize "On"

    filter "configurations:Release"
        defines "MIKU_RELEASE"
        optimize "On" 

    filter "configurations:Dist"
        defines "MIKU_DIST"
        optimize "On"
