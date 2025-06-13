workspace"GameEngine"
    architecture "x64"
    configurations { "Debug", "Release","Dist" }
    startproject "SandBox"

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
        "%{prj.name}/dependency/GLFW_3_4/include",
        "%{prj.name}/dependency/glew",
        "%{prj.name}/src"


        
    }
    
    libdirs {
        "GameEngine/dependency/GLFW_3_4/lib",
        "%{prj.name}/dependency/glew"

    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"
        buildoptions { "/utf-8" }
        
        -- GLFW静态库及其所需的Windows系统库
        links {
            "glfw3",
            "glew32s",  -- GLEW静态库"
            "opengl32",     -- OpenGL库
            "gdi32",        -- Windows GDI库
            "user32",       -- Windows用户库
            "kernel32",     -- Windows内核库
            "shell32"       -- Windows Shell库
        }
        
        defines {
            "ENGINE_PLATFORM_WINDOWS",
            "ENGINE_BUILDING_DLL",
            "GLFW_STATIC"  
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
        "./GameEngine/src"
    }
    
    links {
        "GameEngine"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
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