project "Glad"
    kind "StaticLib"
    language "C"
    
    targetdir ("../../../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir ("../../../bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    
    files {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }
    
    includedirs {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"
        staticruntime "Off"
    
    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
