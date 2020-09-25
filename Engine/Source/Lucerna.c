#include "Lucerna.h"

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <immintrin.h>

#if defined(LC_PLATFORM_LINUX)
#include "GL/gl.h"
#include "X11/Xlib.h"
#include "xcb/xcb.h"
#include "X11/Xlib-xcb.h"
#include "GL/glx.h"
#elif defined(LC_PLATFORM_WINDOWS)
#include "windows.h"
#else
#error "Platform macro not defined"
#endif

#include "Log.c"
#include "GL.c"
#include "Time.c"
#include "Messages.c"
#include "Input.c"
#include "Window.c"
#include "Math.c"
#include "ECS.c"
#include "EcsFunctions.gen.c"
#include "Shader.c"
#include "Camera.c"
#include "AssetLoader.c"
#include "Renderer.c"
#include "EntryPoint.c"
