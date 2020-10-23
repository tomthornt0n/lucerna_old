@echo off

IF "%1"=="Debug" GOTO :DEBUG_BUILD
IF "%1"=="Release" GOTO :RELEASE_BUILD

ECHO Invalid Configuration
EXIT /B 0

:COMMON
PUSHD lcddl
cl /nologo lcddl.c lcddlUserLayer.c /link Kernel32.lib /out:..\bin\lcddl.exe
DEL lcddl.obj
DEL lcddlUserLayer.obj
POPD

COPY /a Client\Assets\Meta\*.lcd Client\Assets\Meta\tmp.lcd
bin\lcddl Client\Assets\Meta\tmp.lcd
DEL Client\Assets\Meta\tmp.lcd

PUSHD AssetPacker
cl /nologo lcap.c /link /out:..\bin\AssetPacker.exe
DEL lcap.obj
POPD

PUSHD bin
.\AssetPacker assets.data
POPD

EXIT /B 0

:DEBUG_BUILD
CALL :COMMON
cl /nologo /Zi /Fd:bin\LucernaProject.pdb /c /I Engine\Include /I Client\Source /I Engine\Source /D LC_PLATFORM_WINDOWS /D LC_DEBUG Engine\Source\Lucerna.c Client\Source\Main.c
link /nologo /debug /subsystem:console Lucerna.obj Main.obj Gdi32.lib User32.lib Shell32.lib opengl32.lib /out:bin\LucernaProject.exe
DEL Main.obj
DEL Lucerna.obj
EXIT /B 0

:RELEASE_BUILD
CALL :COMMON
cl /nologo /c /I Engine\Include /I Client\Source /I Engine\Source /D LC_PLATFORM_WINDOWS /D LC_RELEASE /O2 Engine\Source\Lucerna.c Client\Source\Main.c
link /nologo /debug /subsystem:windows Lucerna.obj Main.obj Gdi32.lib User32.lib Shell32.lib opengl32.lib /out:bin\LucernaProject.exe
DEL Main.obj
DEL Lucerna.obj
EXIT /B 0


