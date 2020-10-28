#ifdef LC_PLATFORM_LINUX
#define _POSIX_C_SOURCE 200809L
#endif

#include "Lucerna.h"

#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <immintrin.h>

#if defined(LC_PLATFORM_LINUX)
#include "gl/glcorearb.h"
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <X11/Xlib-xcb.h>
#include <GL/glx.h>
#include "gl/glxext.h"
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <pthread.h>
#elif defined(LC_PLATFORM_WINDOWS)
#include <windows.h>
#include <windowsx.h>
#include <Fcntl.h>
#include "gl/glcorearb.h"
#include "gl/wglext.h"
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
#include "Renderer.c"
#include "AssetLoader.c"
#include "EntryPoint.c"
