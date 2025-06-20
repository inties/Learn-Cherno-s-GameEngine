workspace"GameEngine"
    architecture "x64"
    configurations { "Debug", "Release","Dist" }
    startproject "SandBox"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/dependency/GLFW_3_4/include"
IncludeDir["Glad"] = "GameEngine/dependency/glad/include"

-- 包含glad子项目
include "GameEngine/dependency/glad"

project "GameEngine"
    location "GameEngine"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    pchheader "pch.h"
    pchsource "GameEngine/src/pch.cpp"
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }
    includedirs {
        "%{prj.name}/dependency/spdlog/include",
        "%{prj.name}/dependency/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{prj.name}/src"

    }
    
    libdirs {
        "%{prj.name}/dependency/GLFW_3_4/lib",
        "%{prj.name}/dependency/glew",
        "%{prj.name}/dependency/libs"

    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }
        
        -- GLFW静态库及其所需的Windows系统库
        links {
            "Glad",
            "glfw3",
            "opengl32.lib",
            "glew32s",
            "user32.lib",
            "gdi32.lib",
            "shell32.lib",
            "kernel32.lib"
        }
        
        defines {
            "ENGINE_PLATFORM_WINDOWS",
            "ENGINE_BUILDING_DLL",
        }
        
        postbuildcommands {
            "{MKDIR} ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/SandBox",
            "{COPYFILE} %{cfg.buildtarget.relpath} ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/SandBox"
        }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "On"
    filter "configurations:Dist"
        defines { "ENGINE_DIST" }
        optimize "On"


project "SandBox"
    location "SandBox"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
 
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }
    includedirs {
        "./GameEngine/dependency/spdlog/include",
        "./GameEngine/dependency/include",
        "./GameEngine/dependency/GLFW_3_4/include",
        "./GameEngine/dependency/glad/include",
        "./GameEngine/src"
    }
    libdirs {
        "./GameEngine/dependency/GLFW_3_4/lib",
        "./GameEngine/dependency/glew",
        "./GameEngine/dependency/libs"
    }
    links {
        "GameEngine",
        "Glad",
        "glfw3"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }
        defines {
            "ENGINE_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "On"
    filter "configurations:Dist"
        defines { "ENGINE_DIST" }
        optimize "On"