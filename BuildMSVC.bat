@echo off

IF "%1"=="Debug" GOTO :DEBUG_BUILD
IF "%1"=="Release" GOTO :RELEASE_BUILD

ECHO Invalid Configuration
EXIT /B %ERRORLEVEL%

:DEBUG_BUILD
CALL :BUILD_LCDDL
CALL :RUN_LCDDL
CALL :BUILD_LUCERNA_DEBUG
EXIT /B %ERRORLEVEL%

:RELEASE_BUILD
CALL :BUILD_LCDDL
CALL :RUN_LCDDL
CALL :BUILD_LUCERNA_RELEASE
EXIT /B %ERRORLEVEL%

:BUILD_LCDDL
PUSHD lcddl
cl /nologo lcddl.c lcddlUserLayer.c /link /out:../bin/lcddl.exe
DEL lcddl.obj
DEL lcddlUserLayer.obj
POPD
EXIT /B 0

:RUN_LCDDL
COPY Client\Assets\Meta\*.lcd Client\Assets\Meta\tmp.lcd
bin\lcddl "Client\Assets\Meta\tmp.lcd"
DEL Client\Assets\Meta\tmp.lcd
EXIT /B 0

:BUILD_LUCERNA_DEBUG
cl /nologo /c /I Engine\Include /I Client\Source /I Engine\Source /D LC_PLATFORM_WINDOWS /D LC_DEBUG /Zi Engine\Source\Lucerna.c Client\Source\Main.c /link /nologo /subsystem:console Gdi32.lib User32.lib Shell32.lib opengl32.lib /out:bin\LucernaProject.exe
DEL Main.obj
EXIT /B 0

:BUILD_LUCERNA_RELEASE
cl /nologo /c /I Engine\Include /I Client\Source /I Engine\Source /D LC_PLATFORM_WINDOWS /D LC_RELEASE /O2 Engine\Source\Lucerna.c Client\Source\Main.c /link /nologo /subsystem:console Gdi32.lib User32.lib Shell32.lib opengl32.lib /out:bin\LucernaProject.exe
DEL Main.obj
EXIT /B 0

