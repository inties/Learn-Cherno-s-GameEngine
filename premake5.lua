workspace"GameEngine"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "SandBox"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "GameEngine/dependency/GLFW_3_4/include"
IncludeDir["Glad"] = "GameEngine/dependency/glad/include"
IncludeDir["ImGui"] = "GameEngine/dependency/imgui"
IncludeDir["glm"] = "GameEngine/dependency/glm_99"
IncludeDir["entt"]="GameEngine/dependency/entt/src/entt"
IncludeDir["nlohmann"]="GameEngine/dependency/nlohmann"
IncludeDir["spdlog"]="GameEngine/dependency/spdlog/include"
IncludeDir["common"]="GameEngine/dependency/include"

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
        "%{IncludeDir.entt}",
        "%{IncludeDir.nlohmann}",
        "%{prj.name}/src",
    }
    libdirs {
        "%{prj.name}/dependency/GLFW_3_4/lib",
        "%{prj.name}/dependency/libs"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "Off"
        systemversion "latest"
        buildoptions { "/utf-8", "/MP" }
   
        -- GLFW静态库及其所需的Windows系统库
        links {
            "ImGui",
            "Glad",
            "glfw3",
            "opengl32.lib",
            "user32.lib",
            "gdi32.lib",
            "shell32.lib",
            "kernel32.lib",
            "assimp-vc143-mtd.lib"
        }
        
        defines {
            "ENGINE_PLATFORM_WINDOWS",
            "ENGINE_BUILDING_DLL",
            "GLAD_GLAPI_EXPORT",
            "GLAD_GLAPI_EXPORT_BUILD"
        }
        
        postbuildcommands {
            "{COPYFILE} %{cfg.buildtarget.relpath} ../"
        }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG" }
        symbols "On"
    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "On"


project "SandBox"
    location "SandBox"
    kind "ConsoleApp"
    language "C++"

    pchheader "pch.h"
    pchsource "SandBox/src/pch.cpp"

    targetdir ("./")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
 
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }
    includedirs {
        "./GameEngine/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.common}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.nlohmann}",
        "%{prj.name}/src",
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
        buildoptions { "/utf-8", "/MP" }
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
