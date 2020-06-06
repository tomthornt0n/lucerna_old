project "DataDeskCustomLayer"
	kind "SharedLib"
	language "C"
    staticruntime "on"
    
    objdir ("bin-int/" .. outputdir)

    files
    {
        "LucernaCustomLayer.c"
    }

    filter "system:windows"
        targetdir ("bin/windows")
        postbuildcommands
        {
            "RunDataDesk.bat"
        }  

    filter "system:linux"
        targetdir ("bin/linux/")
        postbuildcommands
        {
             "./RunDataDesk.sh"
        }

    
    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
        optimize "on"
        
    
