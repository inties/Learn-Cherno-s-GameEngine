workspace"GameEngine"
    architecture "x64"
    configurations { "Debug", "Release","Dist" }
    startproject "SandBox"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/dependency/GLFW_3_4/include"
IncludeDir["Glad"] = "GameEngine/dependency/glad/include"
IncludeDir["ImGui"] = "GameEngine/dependency/imgui"
IncludeDir["glm"] = "GameEngine/dependency/glm_99"

-- 包含glad/imgui两个子项目
include "GameEngine/dependency/glad"
include "GameEngine/dependency/imgui"
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
        "%{prj.name}/src/**.cpp",
    }
    includedirs {
        "%{prj.name}/dependency/spdlog/include",
        "%{prj.name}/dependency/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
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
            "ImGui",
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
            "GLAD_GLAPI_EXPORT",
            "GLAD_GLAPI_EXPORT_BUILD"
        }
        
        postbuildcommands {
            -- "{MKDIR} ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/SandBox",
            "{COPYFILE} %{cfg.buildtarget.relpath} ../"
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

    targetdir ("./")
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
        "%{IncludeDir.glm}",
        "./GameEngine/src"
    }
    libdirs {
        "./GameEngine/dependency/GLFW_3_4/lib",
    }
    links {
        "GameEngine",
        "glfw3"
    }
  
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8" }
        defines {
            "ENGINE_PLATFORM_WINDOWS",
             "GLAD_GLAPI_EXPORT"
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