workspace"GameEngine"
    architecture { "x64"  }
    configuations { "Debug", "Release","Dist" }

project "GameEngine"
    location "GameEngine"
    kind "SharedLib"
    language "C++"

    tagetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir ("bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    ENGINE_PLATFORM_WINDOWS;ENGINE_BUILDING_DLL
    files{
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"

    }