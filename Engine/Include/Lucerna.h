#ifndef LUCERNA_H
#define LUCERNA_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <immintrin.h>

#define arraycount(arr) ((sizeof(arr) / sizeof(*arr)) + 1)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Client Entry Point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcClientMain(int argc, char **argv);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Logging                                                                    
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum
{
    LC_LOG_LEVEL_TRACE,
    LC_LOG_LEVEL_DEBUG,
    LC_LOG_LEVEL_INFO,
    LC_LOG_LEVEL_WARN,
    LC_LOG_LEVEL_ERROR,
    LC_LOG_LEVEL_FATAL
};

#if defined(LC_DEBUG) || defined(LC_DO_NOT_STRIP_LOGGING_FROM_RELEASE)
    #define LC_LOGGING_ENABLED
#endif

#ifdef LC_LOGGING_ENABLED

    #define LC_LOG_ERROR(...) lcLog(LC_LOG_LEVEL_ERROR, "CLIENT", __VA_ARGS__)
    #define LC_LOG_WARN(...)  lcLog(LC_LOG_LEVEL_WARN, "CLIENT", __VA_ARGS__)
    #define LC_LOG_INFO(...)  lcLog(LC_LOG_LEVEL_INFO, "CLIENT", __VA_ARGS__)
    #define LC_LOG_DEBUG(...) lcLog(LC_LOG_LEVEL_DEBUG, "CLIENT", __VA_ARGS__)
    #define LC_LOG_TRACE(...) lcLog(LC_LOG_LEVEL_TRACE, "CLIENT", __VA_ARGS__)

#else

    #define LC_LOG_ERROR(...)
    #define LC_LOG_WARN(...)
    #define LC_LOG_INFO(...)
    #define LC_LOG_DEBUG(...)
    #define LC_LOG_TRACE(...)

#endif

#ifdef _MSC_VER
    #define LC_ASSERT(x, ...) \
    {                                                                          \
        if(!(x))                                                               \
        {                                                                      \
            lcLog(LC_LOG_LEVEL_FATAL,                                          \
                  "Assertion failure at line "__LINE_STRING__" in "__FILE__,   \
                  __VA_ARGS__);                                                \
            __debugbreak();                                                    \
        }                                                                      \
    }
#else
    #define LC_ASSERT(x, ...) \
    {                                                                          \
        if(!(x))                                                               \
        {                                                                      \
            lcLog(LC_LOG_LEVEL_FATAL,                                          \
                  "Assertion failure at line "__LINE_STRING__" in "__FILE__,   \
                  __VA_ARGS__);                                                \
            exit(-1);                                                          \
        }                                                                      \
    }
#endif

void lcLogInit(void);
void lcLogDestroy(void);
void lcLog(int level, char *prefix, char *fmt, ...);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Array Backed List
  
  Author  : Tom Thornton
  Updated : 25 July 2020
  License : MIT, at end of file
  Notes   : Works with standard subscript operator by incrementing the returned
            pointer so that additional information can be stored before the
            array itself.       
                                                                                        
            (uint32_t *)list - 2: number of elements                                    
            (uint32_t *)list - 1: max number of elements
                                                                                        
            To avoid excessive heap allocations, when the list becomes full,
            it is reallocated to be double the size.                                          
                                                                                        
            In addition, when removing elements from the list, the alloacted
            memory is not shrunk untill the number of elements decreases to
            half of the max number of elements.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_LIST_CREATE(list, type) LC_ASSERT((list) == NULL,                   \
                                       "Uninitialised list must be NULL");     \
                             (list) = (type *)                                 \
                             (                                                 \
                                    (uint32_t *)malloc(sizeof(type) +          \
                                    2 * sizeof(uint32_t)) + 2                  \
                             );                                                \
                             *((uint32_t *)(list) - 1) = 1;                    \
                             *((uint32_t *)(list) - 2) = 0

#define LC_LIST_LEN(list) *((uint32_t *)(list) - 2)

#define LC_LIST_PUSH_BACK(list, type, item)                                    \
if (*((uint32_t *)(list) - 2) ==                                               \
    *((uint32_t *)(list) - 1))                                                 \
{                                                                              \
    list = (type *)                                                            \
    (                                                                          \
        (uint32_t *)                                                           \
        (                                                                      \
            realloc((uint32_t *)(list) - 2,                                    \
                   *((uint32_t *)(list) - 1) * 2 * sizeof(type)                \
                   + 2 * sizeof(uint32_t)                                      \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((uint32_t *)(list) - 1) *= 2;                                            \
}                                                                              \
memcpy((list) + *((uint32_t *)(list) - 2),                                     \
       item, sizeof(type));                                                    \
(*((uint32_t *)(list) - 2))++

#define LC_LIST_REMOVE(list, type, index)                                      \
memcpy((list) + index,                                                         \
       (list) + index + 1, (*((uint32_t *)(list) - 1) - (index + 1))           \
                         * sizeof(type));                                      \
(*((uint32_t *)(list) - 2))--;                                                 \
                                                                               \
if (*((uint32_t *)(list) - 2) ==                                               \
    *((uint32_t *)(list) - 1) / 2)                                             \
{                                                                              \
    list = (type *)                                                            \
    (                                                                          \
        (uint32_t *)                                                           \
        (                                                                      \
            realloc((uint32_t *)(list) - 2,                                    \
                   *((uint32_t *)(list) - 1)                                   \
                   / 2                                                         \
                   * sizeof(type)                                              \
                   + 2 * sizeof(uint32_t)                                      \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((uint32_t *)(list) - 1) /= 2;                                            \
}                                                                              \

/* NOTE(tbt): Doesn't deallocate any memory, just resets the element count so
   new entries overwrite the current contents */
#define LC_LIST_CLEAR(list) (*(((uint32_t *)list) - 2) = 0)

#define LC_LIST_DESTROY(list) LC_ASSERT((list) != NULL,                        \
                                        "List is already NULL");               \
                           free((uint32_t *)(list) - 2);                       \
                           list = NULL

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Message System                                                                    
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum
{
    LC_MESSAGE_TYPE_WINDOW_CLOSE,
    LC_MESSAGE_TYPE_WINDOW_RESIZE,

    LC_MESSAGE_TYPE_KEY_PRESS,
    LC_MESSAGE_TYPE_KEY_RELEASE,

    LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS,
    LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE,
    LC_MESSAGE_TYPE_MOUSE_SCROLL,

    LC_MESSAGE_TYPE_COUNT
};

typedef struct
{
    int Type;

    union
    {
        struct lcMessageWindowResize_t
        {
            uint32_t Width, Height;
        } WindowResize;

        struct lcMessageKeyPress_t
        {
            int KeyCode;
            uint8_t Repeat;
        } KeyPress;

        struct lcMessageKeyRelease_t
        {
            int KeyCode;
        } KeyRelease;

        struct lcMessageMouseButtonPress_t
        {
            int KeyCode;
        } MouseButtonPress;

        struct lcMessageMouseButtonRelease_t
        {
            int KeyCode;
        } MouseButtonRelease;

        struct lcMessageMouseScroll_t
        {
            int XOffset, YOffset;
        } MouseScroll;
    };
} lcMessage_t;

typedef void (*lcMessageListener_t)(lcMessage_t);

void lcMessageBind(int messageType, lcMessageListener_t action);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Loader
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : PNG decoder based on https://handmadehero.org/
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int lcLoaderReadFile(const char *filename, uint8_t **result);

typedef struct
{
    uint32_t Width, Height;
    void *Data;
} lcImage_t;

lcImage_t lcLoaderParsePNG(char *filename);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Window
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : Uses GLFW for now, will hopefully write own platform layer at some
            point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcWindowInit(const char *title, int width, int height);
void lcWindowUpdate(void);
void lcWindowSetVSync(bool enabled);
void lcWindowDestroy(void);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Input
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_KEY_SPACE         32
#define LC_KEY_APOSTROPHE    39  
#define LC_KEY_COMMA         44  
#define LC_KEY_MINUS         45  
#define LC_KEY_PERIOD        46  
#define LC_KEY_SLASH         47  
#define LC_KEY_0             48
#define LC_KEY_1             49
#define LC_KEY_2             50
#define LC_KEY_3             51
#define LC_KEY_4             52
#define LC_KEY_5             53
#define LC_KEY_6             54
#define LC_KEY_7             55
#define LC_KEY_8             56
#define LC_KEY_9             57
#define LC_KEY_SEMICOLON     59
#define LC_KEY_EQUAL         61
#define LC_KEY_A             65
#define LC_KEY_B             66
#define LC_KEY_C             67
#define LC_KEY_D             68
#define LC_KEY_E             69
#define LC_KEY_F             70
#define LC_KEY_G             71
#define LC_KEY_H             72
#define LC_KEY_I             73
#define LC_KEY_J             74
#define LC_KEY_K             75
#define LC_KEY_L             76
#define LC_KEY_M             77
#define LC_KEY_N             78
#define LC_KEY_O             79
#define LC_KEY_P             80
#define LC_KEY_Q             81
#define LC_KEY_R             82
#define LC_KEY_S             83
#define LC_KEY_T             84
#define LC_KEY_U             85
#define LC_KEY_V             86
#define LC_KEY_W             87
#define LC_KEY_X             88
#define LC_KEY_Y             89
#define LC_KEY_Z             90
#define LC_KEY_LEFT_BRACKET  91
#define LC_KEY_BACKSLASH     92
#define LC_KEY_RIGHT_BRACKET 93
#define LC_KEY_GRAVE_ACCENT  96
#define LC_KEY_WORLD_1       161
#define LC_KEY_WORLD_2       162

#define LC_KEY_ESCAPE        256
#define LC_KEY_ENTER         257
#define LC_KEY_TAB           258
#define LC_KEY_BACKSPACE     259
#define LC_KEY_INSERT        260
#define LC_KEY_DELETE        261
#define LC_KEY_RIGHT         262
#define LC_KEY_LEFT          263
#define LC_KEY_DOWN          264
#define LC_KEY_UP            265
#define LC_KEY_PAGE_UP       266
#define LC_KEY_PAGE_DOWN     267
#define LC_KEY_HOME          268
#define LC_KEY_END           269
#define LC_KEY_CAPS_LOCK     280
#define LC_KEY_SCROLL_LOCK   281
#define LC_KEY_NUM_LOCK      282
#define LC_KEY_PRINT_SCREEN  283
#define LC_KEY_PAUSE         284
#define LC_KEY_F1            290
#define LC_KEY_F2            291
#define LC_KEY_F3            292
#define LC_KEY_F4            293
#define LC_KEY_F5            294
#define LC_KEY_F6            295
#define LC_KEY_F7            296
#define LC_KEY_F8            297
#define LC_KEY_F9            298
#define LC_KEY_F10           299
#define LC_KEY_F11           300
#define LC_KEY_F12           301
#define LC_KEY_F13           302
#define LC_KEY_F14           303
#define LC_KEY_F15           304
#define LC_KEY_F16           305
#define LC_KEY_F17           306
#define LC_KEY_F18           307
#define LC_KEY_F19           308
#define LC_KEY_F20           309
#define LC_KEY_F21           310
#define LC_KEY_F22           311
#define LC_KEY_F23           312
#define LC_KEY_F24           313
#define LC_KEY_F25           314
#define LC_KEY_KP_0          320
#define LC_KEY_KP_1          321
#define LC_KEY_KP_2          322
#define LC_KEY_KP_3          323
#define LC_KEY_KP_4          324
#define LC_KEY_KP_5          325
#define LC_KEY_KP_6          326
#define LC_KEY_KP_7          327
#define LC_KEY_KP_8          328
#define LC_KEY_KP_9          329
#define LC_KEY_KP_DECIMAL    330
#define LC_KEY_KP_DIVIDE     331
#define LC_KEY_KP_MULTIPLY   332
#define LC_KEY_KP_SUBTRACT   333
#define LC_KEY_KP_ADD        334
#define LC_KEY_KP_ENTER      335
#define LC_KEY_KP_EQUAL      336
#define LC_KEY_LEFT_SHIFT    340
#define LC_KEY_LEFT_CONTROL  341
#define LC_KEY_LEFT_ALT      342
#define LC_KEY_LEFT_SUPER    343
#define LC_KEY_RIGHT_SHIFT   344
#define LC_KEY_RIGHT_CONTROL 345
#define LC_KEY_RIGHT_ALT     346
#define LC_KEY_RIGHT_SUPER   347
#define LC_KEY_MENU          348

#define LC_MOUSE_BUTTON_1      0
#define LC_MOUSE_BUTTON_2      1
#define LC_MOUSE_BUTTON_3      2
#define LC_MOUSE_BUTTON_4      3
#define LC_MOUSE_BUTTON_5      4
#define LC_MOUSE_BUTTON_6      5
#define LC_MOUSE_BUTTON_7      6
#define LC_MOUSE_BUTTON_8      7
#define LC_MOUSE_BUTTON_LAST   LC_MOUSE_BUTTON_8
#define LC_MOUSE_BUTTON_LEFT   LC_MOUSE_BUTTON_1
#define LC_MOUSE_BUTTON_RIGHT  LC_MOUSE_BUTTON_2
#define LC_MOUSE_BUTTON_MIDDLE LC_MOUSE_BUTTON_3

bool lcInputIsKeyPressed(int keycode);
bool lcInputIsMouseButtonPressed(int button);
void lcInputGetMousePos(float *result);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Math
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcVector2Add(float *result, float *vector1, float *vector2);
void lcVector2Subtract(float *result, float *vector1, float *vector2);
void lcVector2Multiply(float *result, float *vector1, float *vector2);
void lcVector2Divide(float *result, float *vector1, float *vector2);
float lcVector2Dot(float *vector1, float *vector2);

void lcVector3Add(float *result, float *vector1, float *vector2);
void lcVector3Subtract(float *result, float *vector1, float *vector2);
void lcVector3Multiply(float *result, float *vector1, float *vector2);
void lcVector3Divide(float *result, float *vector1, float *vector2);
float lcVector3Dot(float *vector1, float *vector2);

void lcVector4Add(float *result, float *vector1, float *vector2);
void lcVector4Subtract(float *result, float *vector1, float *vector2);
void lcVector4Multiply(float *result, float *vector1, float *vector2);
void lcVector4Divide(float *result, float *vector1, float *vector2);
float lcVector4Dot(float *vector1, float *vector2);

void lcMatrix4Multiply(float *result, float *matrix1, float *matrix2);
void lcMatrix4CreateOrthographicProjectionMatrix(float *matrix,
                                                 float left, float right,
                                                 float top, float bottom);
void lcMatrix4CreateTranslationMatrix(float *matrix, float x, float y);
void lcMatrix4CreateScaleMatrix(float *matrix, float x, float y);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ECS
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : Designed initially for simplicity.
            Will eventually get replaced with a better design
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "ECS/Components.gen.h"

#define COMPONENT_RENDERABLE (1 << 0)
typedef struct
{
    float Position1[2];
    float Colour1[4];

    float Position2[2];
    float Colour2[4];

    float Position3[2];
    float Colour3[4];

    float Position4[2];
    float Colour4[4];
} ComponentRenderable;

#define LC_MAX_ENTITIES 10000

#define lcForEntityInSubset(subset)                                            \
int i;                                                                         \
lcEntity_t entity;                                                             \
for(i = 0, entity = (subset).Entities[i];                                      \
    i < LC_LIST_LEN((subset).Entities);                                        \
    ++i, entity = (subset).Entities[i])

typedef uint64_t lcSignature_t;
typedef uint32_t lcEntity_t;

typedef struct
{
    lcSignature_t EntitySignatures[LC_MAX_ENTITIES];

#include "ECS/ComponentArrays.gen.h"

    uint32_t RenderableCount;
    uint32_t EntityToRenderable[LC_MAX_ENTITIES];
    ComponentRenderable ComponentRenderable[LC_MAX_ENTITIES];
} lcScene_t;

typedef struct
{
    lcSignature_t Signature;
    lcEntity_t *Entities;
    lcScene_t *Parent;
} lcSubset_t;

lcScene_t *lcSceneCreate(void);
void lcSceneDestroy(lcScene_t *scene);
lcSubset_t lcSubsetCreate(void);
void lcSubsetDestroy(lcSubset_t subset);
void lcSubsetSetSignature(lcSubset_t *subset, lcSignature_t components);
void lcSubsetRefresh(lcScene_t * scene, lcSubset_t *subset);
lcEntity_t lcEntityCreate(lcScene_t *scene);
void lcEntityDestroy(lcScene_t *scene, lcEntity_t entity);

#include "ECS/EcsFunctions.gen.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Renderer
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef uint32_t lcShader_t;
lcShader_t lcShaderCreate(char *vertexPath, char *fragmentPath);
void lcShaderDestroy(lcShader_t shader);

void ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                              float x, float y,
                              float width, float height,
                              float *colour);
void ComponentRenderableMove(lcScene_t *scene, lcEntity_t entity, 
                             float xOffset, float yOffset);

void lcRendererInit(void);
void lcRendererBindScene(lcScene_t *scene);
void lcRendererBindShader(lcShader_t shader);
void lcRendererRenderToWindow(void);
void lcRendererDestroy(void);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Camera
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcCameraInit(char *uniformName, float *position);
void lcCameraMove(float *offset);
void lcCameraDestroy(void);

#endif

/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

