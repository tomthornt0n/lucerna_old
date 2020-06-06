workspace "Lucerna"

	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}/"

	IncludeDir = {}
	IncludeDir["cglm"] = "Engine/vendor/cglm/"
	IncludeDir["GLFW"] = "Engine/vendor/glfw/include"
	IncludeDir["glad"] = "Engine/vendor/glad/include"
	IncludeDir["stb"]  = "Engine/vendor/stb"

	include "DataDeskCustomLayer"

    include "Engine/vendor/glfw"
    include "Engine/vendor/glad"

	project "LucernaProject"
		location          "Client"
		kind              "ConsoleApp"
		language          "C"
		staticruntime     "on"

		targetdir         ("bin/" .. outputdir)
		objdir            ("bin-int/" .. outputdir)

		files
		{
			"Client/src/Main.c"
		}

        defines
		{
			"GLFW_INCLUDE_NONE"
		}

		includedirs
		{
			"Client/src",
			"Engine/src",
			"%{IncludeDir.cglm}",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.glad}",
			"%{IncludeDir.stb}"
		}

		links
        {
            "glfw",
            "glad"
        }

		filter "system:windows"
			systemversion "latest"
			defines       { "LC_PLATFORM_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
            links         { "opengl32" }

		filter "system:linux"
			defines       "LC_PLATFORM_LINUX"
            links         { "dl", "m", "pthread", "X11", "GL" }

		filter "configurations:Debug"
			defines       "LC_DEBUG"
			runtime       "Debug"
			symbols       "on"

		filter "configurations:Release"
			defines       "LC_RELEASE"
			runtime       "Release"
			optimize      "on"

