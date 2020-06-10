project "glfw"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/" .. outputdir)

	files
	{
        "include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/glfw_config.h",
		"src/egl_context.c",
		"src/context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",
		"src/osmesa_context.c",
		"src/vulkan.c",
		"src/window.c"
	}

	filter "system:windows"
		files
		{
			"src/wgl_context.c",
            "src/win32_init.c",
			"src/win32_joystick.c",
			"src/win32_monitor.c",
			"src/win32_time.c",
			"src/win32_thread.c",
			"src/win32_window.c",
		}

		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}
    filter "system:linux"
		files
		{
			"src/glx_context.c",
			"src/linux_joystick.c",
			"src/posix_thread.c",
			"src/posix_time.c",
            "src/x11_init.c",
			"src/x11_monitor.c",
			"src/x11_window.c",
			"src/xkb_unicode.c"
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"