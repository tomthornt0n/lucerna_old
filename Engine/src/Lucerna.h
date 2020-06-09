#ifndef LUCERNA_H
#define LUCERNA_H

//c standard library
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//dependencies
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "mathc.h"

#include "stretchy_buffer.h"

//lucerna engine
#include "Core/Log.c"
#include "Core/Util.c"
#include "Core/Window.c"
#include "Core/Input.c"
#include "Core/Math.c"

#include "ECS/Components.gen.c"
#include "ECS/BuiltInComponents.c"
#include "ECS/ECS.c"

#include "Renderer/Shader.c"
#include "Renderer/Renderer.c"

#endif

