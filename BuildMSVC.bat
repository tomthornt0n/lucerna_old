@echo off

IF "%1"=="Debug" CALL :DEBUG_BUILD
IF "%1"=="Release" CALL :RELEASE_BUILD

ECHO Invalid Configuration
EXIT /B 0

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

cl /nologo /c /I Engine\Include /I Client\Source /I Engine\Source /D LC_PLATFORM_WINDOWS %COMPILE_FLAGS% Engine\Source\Lucerna.c Client\Source\Main.c
link /nologo /stack:4194304 %LINK_FLAGS% Lucerna.obj Main.obj Gdi32.lib User32.lib Shell32.lib opengl32.lib /out:bin\LucernaProject.exe

DEL Main.obj
DEL Lucerna.obj

EXIT /B 0

:DEBUG_BUILD
set COMPILE_FLAGS=/D LC_DEBUG /Zi /Fd:bin\LucernaProject.pdbset LINK_FLAGS=/debug /subsystem:console
EXIT /B 0

:RELEASE_BUILD
set COMPILE_FLAGS= /D LC_RELEASE /O2
set LINK_FLAGS=/subsystem:windows 
EXIT /B 0


