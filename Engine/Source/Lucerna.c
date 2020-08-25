#include "Lucerna.h"

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

#include "Core/Log.c"
#include "Core/GL.c"
#include "Core/Time.c"
#include "Core/Messages.c"
#include "Core/Loader.c"
#include "Core/Input.c"
#include "Core/Window.c"
#include "Core/Math.c"

#include "ECS/ECS.c"
#include "ECS/EcsFunctions.gen.c"

#include "Renderer/Shader.c"
#include "Renderer/Camera.c"
#include "Renderer/Renderer.c"

#include "Core/EntryPoint.c"
