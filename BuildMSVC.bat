@echo off

IF "%1"=="Debug" GOTO :DEBUG_BUILD
IF "%1"=="Release" GOTO :RELEASE_BUILD

ECHO Invalid Configuration
EXIT /B %ERRORLEVEL%

:DEBUG_BUILD
CALL :BUILD_LCDDL
CALL :BUILD_GLAD
CALL :BUILD_GLFW_DEBUG
CALL :RUN_LCDDL
CALL :BUILD_LUCERNA_DEBUG
EXIT /B %ERRORLEVEL%

:RELEASE_BUILD
CALL :BUILD_LCDDL
CALL :BUILD_GLAD
CALL :BUILD_GLFW_RELEASE
CALL :RUN_LCDDL
CALL :BUILD_LUCERNA_RELEASE
EXIT /B %ERRORLEVEL%

:BUILD_LCDDL
PUSHD lcddl
cl /nologo lcddl.c lcddlUserLayer.c /link /out:../bin/lcddl.exe
popd
EXIT /B 0

:RUN_LCDDL
PUSHD bin
lcddl ../Client/Assets/Meta/*.lcd
POPD
EXIT /B 0

:BUILD_GLAD
PUSHD Engine\Vendor\glad
cl /nologo /c /I include src\glad.c /Fo:bin-int\glad.obj
lib /nologo bin-int\glad.obj /out:bin\glad.lib
POPD
EXIT /B 0

:BUILD_GLFW_DEBUG
PUSHD Engine\Vendor\glfw
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/egl_context.c /Fo:bin-int/egl_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/context.c /Fo:bin-int/context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/init.c /Fo:bin-int/init.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/input.c /Fo:bin-int/input.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/monitor.c /Fo:bin-int/monitor.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/osmesa_context.c /Fo:bin-int/osmesa_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/vulkan.c /Fo:bin-int/vulkan.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/window.c /Fo:bin-int/window.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/wgl_context.c /Fo:bin-int/wgl_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_init.c /Fo:bin-int/win32_init.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_joystick.c /Fo:bin-int/win32_joystick.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_monitor.c /Fo:bin-int/win32_monitor.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_time.c /Fo:bin-int/win32_time.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_thread.c /Fo:bin-int/win32_thread.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /Zi src/win32_window.c /Fo:bin-int/win32_window.obj
lib /nologo bin-int\*.obj /out:bin\glfw.lib
POPD
EXIT /B 0


:BUILD_GLFW_RELEASE
PUSHD Engine\Vendor\glfw
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/egl_context.c /Fo:bin-int/egl_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/context.c /Fo:bin-int/context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/init.c /Fo:bin-int/init.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/input.c /Fo:bin-int/input.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/monitor.c /Fo:bin-int/monitor.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/osmesa_context.c /Fo:bin-int/osmesa_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/vulkan.c /Fo:bin-int/vulkan.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/window.c /Fo:bin-int/window.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/wgl_context.c /Fo:bin-int/wgl_context.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_init.c /Fo:bin-int/win32_init.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_joystick.c /Fo:bin-int/win32_joystick.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_monitor.c /Fo:bin-int/win32_monitor.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_time.c /Fo:bin-int/win32_time.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_thread.c /Fo:bin-int/win32_thread.obj
cl /nologo /c /D _GLFW_WIN32 /D _CRT_SECURE_NO_WARNINGS /O2 src/win32_window.c /Fo:bin-int/win32_window.obj
lib /nologo bin-int\*.obj /out:bin\glfw.lib
POPD
EXIT /B 0

:BUILD_LUCERNA_DEBUG
cl /nologo /c /I Engine\Vendor\glfw\include /I Engine\Vendor\glad\include /I Client\Source /I Engine\Source /D GLFW_INCLUDE_NONE /D LC_PLATFORM_WINDOWS /D LC_DEBUG /Zi /Fo:bin\Main.obj Engine\Source\Lucerna.c Client\Source\Main.c Engine\Source\Lucerna.c /Fd:bin\LucernaProject.pdb
link /nologo /subsystem:console Gdi32.lib User32.lib Shell32.lib opengl32.lib Engine\Vendor\glfw\bin\glfw.lib Engine\Vendor\glad\bin\glad.lib bin\main.obj /out:bin\LucernaProject.exe
EXIT /B 0

:BUILD_LUCERNA_RELEASE
cl /nologo /c /I Engine\Vendor\glfw\include /I Engine\Vendor\glad\include /I Client\Source /I Engine\Source /D GLFW_INCLUDE_NONE /D LC_PLATFORM_WINDOWS /D LC_DEBUG /O2 /Fo:bin\Main.obj Engine\Source\Lucerna.c Client\Source\Main.c Engine\Source\Lucerna.c /Fd:bin\LucernaProject.pdb
link /nologo /subsystem:console Gdi32.lib User32.lib Shell32.lib opengl32.lib Engine\Vendor\glfw\bin\glfw.lib Engine\Vendor\glad\bin\glad.lib bin\main.obj /out:bin\LucernaProject.exe
EXIT /B 0
