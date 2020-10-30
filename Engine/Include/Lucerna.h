#ifndef LUCERNA_H
#define LUCERNA_H

#include <errno.h>

extern int errno;

#define LC_ARRAY_COUNT(arr) ((sizeof(arr) / sizeof(*arr)) + 1)

#define true 1
#define false 0

#define internal static

/* NOTE(tbt): can make assumptions about widths as the platform will remain
              relatively constant.
*/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef char           i8;
typedef short int      i16;
typedef int            i32;
typedef long           i64;

typedef float          f32;
typedef double         f64;

typedef unsigned char  b8;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Client Entry Point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_WINDOW_TITLE_MAX_LEN 64

typedef struct
{
    i8 WindowTitle[LC_WINDOW_TITLE_MAX_LEN];
    u32 WindowDimensions[2];
    b8 VSyncEnabled;

    f32 CameraPosition[2];
} lcInitConfig_t;

lcInitConfig_t lcClientConfig(void);
void lcClientMain(i32 argc, i8 **argv);

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
    #define LC_ASSERT(x, ...)                                                  \
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
    #define LC_ASSERT(x, ...)                                                  \
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
void lcLog(i32 level, char *prefix, char *fmt, ...);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Time                                                                    
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : Not syncrhonised in anyway, should only be used for relative
            measurements such as timesteps or performance counters.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

u64 lcClockGetTime(void);

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
            it is reallocated to be double the size (as opposed to resizing it
            to fit).                                          
                                                                                        
            In addition, when removing elements from the list, the alloacted
            memory is not shrunk untill the number of elements decreases to
            half of it's current size, reducing the number of allocations
            required should more elements be added back into the list.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_LIST_CREATE(list) LC_ASSERT((list) == NULL,                   \
                                       "Uninitialised list must be NULL");     \
                             (list) = (void *)                                 \
                             (                                                 \
                                    (u32 *)malloc(sizeof(*list) +               \
                                    2 * sizeof(u32)) + 2                       \
                             );                                                \
                             *((u32 *)(list) - 1) = 1;                         \
                             *((u32 *)(list) - 2) = 0

#define LC_LIST_LEN(list) *((u32 *)(list) - 2)

#define LC_LIST_PUSH_BACK(list, item)                                    \
if (*((u32 *)(list) - 2) ==                                                    \
    *((u32 *)(list) - 1))                                                      \
{                                                                              \
    list = (void *)                                                            \
    (                                                                          \
        (u32 *)                                                                \
        (                                                                      \
            realloc((u32 *)(list) - 2,                                         \
                   *((u32 *)(list) - 1) * 2 * sizeof(*(list))                     \
                   + 2 * sizeof(u32)                                           \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((u32 *)(list) - 1) *= 2;                                                 \
}                                                                              \
memcpy((list) + *((u32 *)(list) - 2),                                          \
       item, sizeof(*(list)));                                                    \
(*((u32 *)(list) - 2))++

#define LC_LIST_REMOVE(list, index)                                      \
memcpy((list) + index,                                                         \
       (list) + index + 1, (*((u32 *)(list) - 1) - (index + 1))                \
                         * sizeof(*(list)));                                      \
(*((u32 *)(list) - 2))--;                                                      \
                                                                               \
if (*((u32 *)(list) - 2) ==                                                    \
    *((u32 *)(list) - 1) / 2)                                                  \
{                                                                              \
    list = (void *)                                                            \
    (                                                                          \
        (u32 *)                                                                \
        (                                                                      \
            realloc((u32 *)(list) - 2,                                         \
                   *((u32 *)(list) - 1)                                        \
                   / 2                                                         \
                   * sizeof(type)                                              \
                   + 2 * sizeof(u32)                                           \
                   )                                                           \
        ) + 2                                                                  \
    );                                                                         \
    *((u32 *)(list) - 1) /= 2;                                                 \
}                                                                              \

/* NOTE(tbt): Doesn't deallocate any memory, just resets the element count so
   new entries overwrite the current contents */
#define LC_LIST_CLEAR(list) (*(((u32 *)list) - 2) = 0)

#define LC_LIST_DESTROY(list) LC_ASSERT((list) != NULL,                        \
                                        "List is already NULL");               \
                           free((u32 *)(list) - 2);                            \
                           list = NULL

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Message System                                                                    
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : Not threadsafe. Only intended to allow easy communication about
            engine events with the client.
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
    i32 Type;
} lcGenericMessage_t;

typedef struct
{
    lcGenericMessage_t Header;
    u32 Width;
    u32 Height;
} lcWindowResizeMessage_t;

struct lcInputMessage_t
{
    lcGenericMessage_t Header;
    i32 KeyCode;
};
struct lcMouseScrollMessage_t
{
    lcGenericMessage_t Header;
    i32 ScrollOffset;
};

typedef struct lcInputMessage_t lcKeyPressMessage_t;
typedef struct lcInputMessage_t lcKeyReleaseMessage_t;
typedef struct lcInputMessage_t lcMouseButtonPressMessage_t;
typedef struct lcInputMessage_t lcMouseButtonReleaseMessage_t;
typedef struct lcMouseScrollMessage_t lcMouseScrollMessage_t;

typedef void (*lcMessageHandler_t)(lcGenericMessage_t *);

void lcMessageBind(i32 messageType, lcMessageHandler_t handler);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Window
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcWindowUpdate(void);
void lcWindowSetVSync(b8 enabled);
void lcWindowGetSize(u32 *result);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Input
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_KEY_NONE                0
#define LC_KEY_A                   4
#define LC_KEY_B                   5
#define LC_KEY_C                   6
#define LC_KEY_D                   7
#define LC_KEY_E                   8
#define LC_KEY_F                   9
#define LC_KEY_G                   10
#define LC_KEY_H                   11
#define LC_KEY_I                   12
#define LC_KEY_J                   13
#define LC_KEY_K                   14
#define LC_KEY_L                   15
#define LC_KEY_M                   16
#define LC_KEY_N                   17
#define LC_KEY_O                   18
#define LC_KEY_P                   19
#define LC_KEY_Q                   20
#define LC_KEY_R                   21
#define LC_KEY_S                   22
#define LC_KEY_T                   23
#define LC_KEY_U                   24
#define LC_KEY_V                   25
#define LC_KEY_W                   26
#define LC_KEY_X                   27
#define LC_KEY_Y                   28
#define LC_KEY_Z                   29
#define LC_KEY_1                   30
#define LC_KEY_2                   31
#define LC_KEY_3                   32
#define LC_KEY_4                   33
#define LC_KEY_5                   34
#define LC_KEY_6                   35
#define LC_KEY_7                   36
#define LC_KEY_8                   37
#define LC_KEY_9                   38
#define LC_KEY_0                   39
#define LC_KEY_ESCAPE              41
#define LC_KEY_DELETE              42
#define LC_KEY_TAB                 43
#define LC_KEY_SPACE               44
#define LC_KEY_MINUS               45
#define LC_KEY_EQUALS              46
#define LC_KEY_LEFT_BRACKET        47
#define LC_KEY_RIGHT_BRACKET       48
#define LC_KEY_BACKSLASH           49
#define LC_KEY_SEMICOLON           51
#define LC_KEY_QUOTE               52
#define LC_KEY_GRAVE               53
#define LC_KEY_COMMA               54
#define LC_KEY_PERIOD              55
#define LC_KEY_SLASH               56
#define LC_KEY_CAPS_LOCK           57
#define LC_KEY_F1                  58
#define LC_KEY_F2                  59
#define LC_KEY_F3                  60
#define LC_KEY_F4                  61
#define LC_KEY_F5                  62
#define LC_KEY_F6                  63
#define LC_KEY_F7                  64
#define LC_KEY_F8                  65
#define LC_KEY_F9                  66
#define LC_KEY_F10                 67
#define LC_KEY_F11                 68
#define LC_KEY_F12                 69
#define LC_KEY_PRINT_SCREEN        70
#define LC_KEY_SCROLL_LOCK         71
#define LC_KEY_PAUSE               72
#define LC_KEY_INSERT              73
#define LC_KEY_HOME                74
#define LC_KEY_PAGEUP              75
#define LC_KEY_DEL                 76
#define LC_KEY_END                 77
#define LC_KEY_PAGE_DOWN           78
#define LC_KEY_RIGHT               79
#define LC_KEY_LEFT                80
#define LC_KEY_DOWN                81
#define LC_KEY_UP                  82
#define LC_KEY_KP_NUM_LOCK         83
#define LC_KEY_KP_DIVIDE           84
#define LC_KEY_KP_MULTIPLY         85
#define LC_KEY_KP_SUBTRACT         86
#define LC_KEY_KP_ADD              87
#define LC_KEY_KP_ENTER            88
#define LC_KEY_KP_1                89
#define LC_KEY_KP_2                90
#define LC_KEY_KP_3                91
#define LC_KEY_KP_4                92
#define LC_KEY_KP_5                93
#define LC_KEY_KP_6                94
#define LC_KEY_KP_7                95
#define LC_KEY_KP_8                96
#define LC_KEY_KP_9                97
#define LC_KEY_KP_0                98
#define LC_KEY_KP_POINT            99
#define LC_KEY_NON_US_BACKSLASH    100
#define LC_KEY_KP_EQUALS           103
#define LC_KEY_F13                 104
#define LC_KEY_F14                 105
#define LC_KEY_F15                 106
#define LC_KEY_F16                 107
#define LC_KEY_F17                 108
#define LC_KEY_F18                 109
#define LC_KEY_F19                 110
#define LC_KEY_F20                 111
#define LC_KEY_F21                 112
#define LC_KEY_F22                 113
#define LC_KEY_F23                 114
#define LC_KEY_F24                 115
#define LC_KEY_HELP                117
#define LC_KEY_MENU                118
#define LC_KEY_MUTE                127
#define LC_KEY_SYS_REQ             154
#define LC_KEY_RETURN              158
#define LC_KEY_KP_CLEAR            216
#define LC_KEY_KP_DECIMAL          220
#define LC_KEY_LEFT_CONTROL        224
#define LC_KEY_LEFT_SHIFT          225
#define LC_KEY_LEFT_ALT            226
#define LC_KEY_LEFT_SUPER          227
#define LC_KEY_RIGHT_CONTROL       228
#define LC_KEY_RIGHT_SHIFT         229
#define LC_KEY_RIGHT_ALT           230
#define LC_KEY_RIGHT_SUPER         231

#define LC_MOUSE_BUTTON_1          0
#define LC_MOUSE_BUTTON_2          1
#define LC_MOUSE_BUTTON_3          2
#define LC_MOUSE_BUTTON_4          3
#define LC_MOUSE_BUTTON_5          4
#define LC_MOUSE_BUTTON_6          5
#define LC_MOUSE_BUTTON_7          6
#define LC_MOUSE_BUTTON_8          7
#define LC_MOUSE_BUTTON_LEFT       LC_MOUSE_BUTTON_1
#define LC_MOUSE_BUTTON_MIDDLE     LC_MOUSE_BUTTON_2
#define LC_MOUSE_BUTTON_RIGHT      LC_MOUSE_BUTTON_3
#define LC_MOUSE_SCROLL_UP         LC_MOUSE_BUTTON_5
#define LC_MOUSE_SCROLL_DOWN       LC_MOUSE_BUTTON_6

extern b8 lcInputIsKeyPressed[256];
extern b8 lcInputIsMouseButtonPressed[8];
extern i16 lcInputMousePosition[2];

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Math
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcVector2Add(f32 *result, f32 *vector1, f32 *vector2);
void lcVector2Subtract(f32 *result, f32 *vector1, f32 *vector2);
void lcVector2Multiply(f32 *result, f32 *vector1, f32 *vector2);
void lcVector2Divide(f32 *result, f32 *vector1, f32 *vector2);
f32 lcVector2Dot(f32 *vector1, f32 *vector2);

void lcVector3Add(f32 *result, f32 *vector1, f32 *vector2);
void lcVector3Subtract(f32 *result, f32 *vector1, f32 *vector2);
void lcVector3Multiply(f32 *result, f32 *vector1, f32 *vector2);
void lcVector3Divide(f32 *result, f32 *vector1, f32 *vector2);
f32 lcVector3Dot(f32 *vector1, f32 *vector2);

void lcVector4Add(f32 *result, f32 *vector1, f32 *vector2);
void lcVector4Subtract(f32 *result, f32 *vector1, f32 *vector2);
void lcVector4Multiply(f32 *result, f32 *vector1, f32 *vector2);
void lcVector4Divide(f32 *result, f32 *vector1, f32 *vector2);
f32 lcVector4Dot(f32 *vector1, f32 *vector2);

void lcMatrix4Multiply(f32 *result, f32 *matrix1, f32 *matrix2);
void lcMatrix4CreateOrthographicProjectionMatrix(f32 *matrix,
                                                 f32 left, f32 right,
                                                 f32 top, f32 bottom);
void lcMatrix4CreateTranslationMatrix(f32 *matrix, f32 x, f32 y);
void lcMatrix4CreateScaleMatrix(f32 *matrix, f32 x, f32 y);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Asset Loader
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : Loads assets from files generated by the asset packer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* include structs representing assets in file */
#include "../../AssetPacker/lcap.h"

enum
{
    LC_ASSET_TYPE_SPRITE = LCAP_ASSET_TYPE_SPRITE,
    LC_ASSET_TYPE_SHADER = LCAP_ASSET_TYPE_SHADER,
    LC_ASSET_TYPE_SOUND  = LCAP_ASSET_TYPE_SOUND
};


void *lcLoadAsset(i8 *name, i32 type);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Camera
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcCameraMove(f32 *offset);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Audio
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct _lcAudioSource_t lcAudioSource_t;

void lcAudioPlay(lcAudioSource_t *source);
void lcAudioStop(lcAudioSource_t *source);
void lcAudioPause(lcAudioSource_t *source);

void lcAudioSetLooping(lcAudioSource_t *source, b8 looping);
void lcAudioSetLevel(lcAudioSource_t *source, f32 level);
void lcAudioSetPan(lcAudioSource_t *source, f32 pan);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ECS
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : N/A
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "Components.gen.h"

#define LC_MAX_ENTITIES 10000

#define COMPONENT_RENDERABLE (1 << 0)
typedef struct
{
    f32 Position1[2];
    f32 Colour1[4];
    f32 TexCoords1[2];

    f32 Position2[2];
    f32 Colour2[4];
    f32 TexCoords2[2];

    f32 Position3[2];
    f32 Colour3[4];
    f32 TexCoords3[2];

    f32 Position4[2];
    f32 Colour4[4];
    f32 TexCoords4[2];
} ComponentRenderable;

#define lcForEntityInSubset(index, entity, subset)                             \
for(index = 0, entity = (subset).Entities[index];                              \
    index < LC_LIST_LEN((subset).Entities);                                    \
    ++index, entity = (subset).Entities[index])

typedef u64 lcSignature_t;
typedef u32 lcEntity_t;

typedef struct
{
    lcSignature_t EntitySignatures[LC_MAX_ENTITIES];

#include "ComponentArrays.gen.h"

    u64 RenderableCount;
    u64 EntityToRenderable[LC_MAX_ENTITIES];
    ComponentRenderable ComponentRenderable[LC_MAX_ENTITIES];
} lcScene_t;

typedef struct
{
    lcSignature_t Signature;
    lcEntity_t *Entities;
    lcScene_t *Parent;
} lcSubset_t;

void lcSceneCreate(lcScene_t *scene);
void lcSceneDestroy(lcScene_t *scene);
lcSubset_t lcSubsetCreate(void);
void lcSubsetDestroy(lcSubset_t subset);
void lcSubsetSetSignature(lcSubset_t *subset, lcSignature_t components);
void lcSubsetRefresh(lcScene_t * scene, lcSubset_t *subset);
lcEntity_t lcEntityCreate(lcScene_t *scene);
void lcEntityDestroy(lcScene_t *scene, lcEntity_t entity);

#include "EcsFunctions.gen.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Renderer
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_SHADER_PATH(shader) "../Client/Assets/Shaders/" shader

typedef struct
{
    f32 Min[2];
    f32 Max[2];
} lcSprite_t;

typedef u64 lcShader_t;

void ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                            f32 x, f32 y,
                            f32 width, f32 height,
                            f32 *colour,
                            lcSprite_t *texture);
void ComponentRenderableMove(lcScene_t *scene, lcEntity_t entity, 
                             f32 xOffset, f32 yOffset);

lcShader_t lcShaderCreate(i8 *vertexPath, i8 *fragmentPath);
void lcShaderDestroy(lcShader_t shader);

void lcRendererBindScene(lcScene_t *scene);
void lcRendererBindShader(lcShader_t shader);
void lcRendererRenderToWindow(void);

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


