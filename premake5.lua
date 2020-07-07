workspace "Lucerna"

    architecture "x64"
    
    configurations
    {
        "Debug",
        "Release"
    }
    
    outputdir = "%{cfg.buildcfg}-%{cfg.system}/"
    
    IncludeDir = {}
    IncludeDir["GLFW"]   = "Engine/Vendor/glfw/include"
    IncludeDir["glad"]   = "Engine/Vendor/glad/include"
    
    include "DataDeskCustomLayer"

    include "Engine/Vendor/glfw"
    include "Engine/Vendor/glad"

    project "LucernaProject"
        location            "Client"
        kind                "ConsoleApp"
        language            "C"
        staticruntime       "on"
        
        targetdir         ( "bin/" .. outputdir )
        objdir            ( "bin-int/" .. outputdir )
        
        files
        {
            "Client/Source/Main.c"
        }
        
        defines
    	{
    	    "GLFW_INCLUDE_NONE"
    	}
    
    	includedirs
    	{
    	    "Client/Source",
    	    "Engine/Source",
    	    "%{IncludeDir.GLFW}",
    	    "%{IncludeDir.glad}",
    	}
    
    	links
        {
            "glfw",
            "glad"
        }
    
        filter "system:windows"
            systemversion   "latest"
            defines       { "LC_PLATFORM_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
            links         { "opengl32" }
    
        filter "system:linux"
            buildoptions  { "-std=c89" }
    	    defines       { "LC_PLATFORM_LINUX" }
            links         { "dl", "m", "pthread", "X11", "GL" }
    
    	filter "configurations:Debug"
    	    defines         "LC_DEBUG"
    	    runtime         "Debug"
    	    symbols         "on"
    
        filter {"configurations:Debug", "system:linux"}
            buildoptions  { "-g" }
    
        filter "configurations:Release"
            defines         "LC_RELEASE"
    	    runtime         "Release"
    	    optimize        "on"
    
