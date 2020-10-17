#ifndef LUCERNA_H
#define LUCERNA_H

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

extern int errno;

#define LC_ARRAY_COUNT(arr) ((sizeof(arr) / sizeof(*arr)) + 1)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Client Entry Point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_WINDOW_TITLE_MAX_LEN 64

typedef struct
{
    char WindowTitle[LC_WINDOW_TITLE_MAX_LEN];
    unsigned int WindowDimensions[2];
    bool VSyncEnabled;

    float CameraPosition[2];
} lcInitConfig_t;

lcInitConfig_t lcClientConfig(void);
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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Time                                                                    
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

uint64_t lcClockGetTime(void);

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
} lcGenericMessage_t;

typedef struct
{
    lcGenericMessage_t Header;
    uint32_t Width;
    uint32_t Height;
} lcWindowResizeMessage_t;

struct lcInputMessage_t
{
    lcGenericMessage_t Header;
    union
    {
        int KeyCode;
        int ScrollOffset;
    };
};
typedef struct lcInputMessage_t lcKeyPressMessage_t;
typedef struct lcInputMessage_t lcKeyReleaseMessage_t;
typedef struct lcInputMessage_t lcMouseButtonPressMessage_t;
typedef struct lcInputMessage_t lcMouseButtonReleaseMessage_t;
typedef struct lcInputMessage_t lcMouseScrollMessage_t;

typedef void (*lcMessageListener_t)(lcGenericMessage_t *);

void lcMessageBind(int messageType, lcMessageListener_t action);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Window
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcWindowUpdate(void);
void lcWindowSetVSync(bool enabled);
void lcWindowGetSize(uint32_t *result);

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

extern bool    lcInputIsKeyPressed[256];
extern bool    lcInputIsMouseButtonPressed[8];
extern int16_t lcInputMousePosition[2];

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

#include "Components.gen.h"

#define LC_MAX_ENTITIES 10000

#define COMPONENT_RENDERABLE (1 << 0)
typedef struct
{
    float Position1[2];
    float Colour1[4];
    float TexCoords1[2];

    float Position2[2];
    float Colour2[4];
    float TexCoords2[2];

    float Position3[2];
    float Colour3[4];
    float TexCoords3[2];

    float Position4[2];
    float Colour4[4];
    float TexCoords4[2];
} ComponentRenderable;

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

#include "ComponentArrays.gen.h"

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

#include "EcsFunctions.gen.h"

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
    /* ... */
};

typedef struct
{
    int Type;
} lcGenericAsset_t;

typedef struct
{
    lcGenericAsset_t Header;

    float Min[2];
    float Max[2];
} lcAssetSprite_t;

lcGenericAsset_t *lcLoadAsset(char *name);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Renderer
  
  Author  : Tom Thornton
  License : MIT, at end of file
  Notes   : NA
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LC_SHADER_PATH(shader) "../Client/Assets/Shaders/" shader

void ComponentRenderableAdd(lcScene_t *scene, lcEntity_t entity,
                            float x, float y,
                            float width, float height,
                            float *colour,
                            lcAssetSprite_t *texture);
void ComponentRenderableMove(lcScene_t *scene, lcEntity_t entity, 
                             float xOffset, float yOffset);

typedef uint32_t lcShader_t;
lcShader_t lcShaderCreate(char *vertexPath, char *fragmentPath);
void lcShaderDestroy(lcShader_t shader);

void lcRendererBindScene(lcScene_t *scene);
void lcRendererBindShader(lcShader_t shader);
void lcRendererRenderToWindow(void);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Camera
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void lcCameraMove(float *offset);

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

