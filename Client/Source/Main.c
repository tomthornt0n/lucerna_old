#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lucerna.h"

/* NOTE(tbt): Everything about this example game is terrible */

#define PlayerAcceleration 40.0f
#define PlayerSpeed 450.0f
#define OponentSpeed 450.0f
#define OponentAcceleration 40.0f
#define OponentBallRange 0.0f
#define BallSpeed 500.0f

static uint8_t g_running = 1;
static float g_winYBounds[2]; /* top and bottom of screen so the ball can bounce */
static float g_winXBounds[2]; /* left and right of screen */

static void
OnWindowClose(lcGenericMessage_t *message)
{
    g_running = 0;
}

static void
OnWindowResize(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize = (lcWindowResizeMessage_t *)message;

    g_winYBounds[0] = -((float)resize->Height / 2.0f);
    g_winYBounds[1] = +((float)resize->Height / 2.0f);

    g_winXBounds[0] = -((float)resize->Width / 2.0f);
    g_winXBounds[1] = +((float)resize->Width / 2.0f);
}

static void
UpdatePhysics(lcSubset_t *subset,
              lcScene_t *scene,
              double frameTime)
{
    lcForEntityInSubset(*subset)
    {
        scene->ComponentPhysics[entity].Minimum[0] +=
        scene->ComponentPhysics[entity].Velocity[0] *
        frameTime;

        scene->ComponentPhysics[entity].Minimum[1] +=
        scene->ComponentPhysics[entity].Velocity[1] *
        frameTime;

        scene->ComponentPhysics[entity].Maximum[0] +=
        scene->ComponentPhysics[entity].Velocity[0] *
        frameTime;

        scene->ComponentPhysics[entity].Maximum[1] +=
        scene->ComponentPhysics[entity].Velocity[1] *
        frameTime;

        ComponentRenderableMove(scene, entity,
                                scene->ComponentPhysics[entity].Velocity[0] *
                                frameTime,
                                scene->ComponentPhysics[entity].Velocity[1] *
                                frameTime);
    }
}

static void
UpdateComputerPaddle(lcScene_t *scene,
                     lcEntity_t ball,
                     lcEntity_t paddle)
{
    if (scene->ComponentPhysics[paddle].Minimum[1] >
        scene->ComponentPhysics[ball].Maximum[1] + OponentBallRange &&
        scene->ComponentPhysics[paddle].Velocity[1] > -OponentSpeed)
    {
        scene->ComponentPhysics[paddle].Velocity[1] -= OponentAcceleration;
    }
    else if (scene->ComponentPhysics[paddle].Maximum[1] <
             scene->ComponentPhysics[ball].Minimum[1] - OponentBallRange &&
             scene->ComponentPhysics[ball].Velocity[1] < OponentSpeed)
    {
        scene->ComponentPhysics[paddle].Velocity[1] += OponentAcceleration;
    }
}

static void
UpdatePlayerPaddle(lcScene_t *scene,
                   lcEntity_t paddle)
{
    if (lcInputIsKeyPressed[LC_KEY_S] &&
        scene->ComponentPhysics[paddle].Velocity[1] < PlayerSpeed)
    {
        scene->ComponentPhysics[paddle].Velocity[1] += PlayerAcceleration;
    }
    else if (lcInputIsKeyPressed[LC_KEY_W] &&
             scene->ComponentPhysics[paddle].Velocity[1] > -PlayerSpeed)
    {
        scene->ComponentPhysics[paddle].Velocity[1] -= PlayerAcceleration;
    }
    else
    {
        if (scene->ComponentPhysics[paddle].Velocity[1] < -0.05f)
        {
            scene->ComponentPhysics[paddle].Velocity[1] += PlayerAcceleration;
        }
        else if (scene->ComponentPhysics[paddle].Velocity[1] > 0.05f)
        {
            scene->ComponentPhysics[paddle].Velocity[1] -= PlayerAcceleration;
        }
        else
        {
            scene->ComponentPhysics[paddle].Velocity[1] = 0.0f;
        }
    }
}

static void
UpdateBall(lcScene_t *scene,
           lcSubset_t *physicsSubset,
           lcEntity_t ball)
{
    float *ballMin = scene->ComponentPhysics[ball].Minimum;
    float *ballMax = scene->ComponentPhysics[ball].Maximum;

    if (ballMin[1] < g_winYBounds[0])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
    }
    else if (ballMax[1] > g_winYBounds[1])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
    }

    if (ballMin[0] < g_winXBounds[0])
    {
        LC_LOG_INFO("Computer wins... :(");
        exit(0);
    }
    else if (ballMax[0] > g_winXBounds[1])
    {
        LC_LOG_INFO("Player wins! :)");
        exit(0);
    }

    lcForEntityInSubset(*physicsSubset)
    {
        if (entity != ball)
        {
            float *min = scene->ComponentPhysics[entity].Minimum;
            float *max = scene->ComponentPhysics[entity].Maximum;

            if (!(max[0] < ballMin[0] || min[0] > ballMax[0]) &&
                !(max[1] < ballMin[1] || min[1] > ballMax[1]))
            {
                scene->ComponentPhysics[ball].Velocity[0] *= -1.0f;
                scene->ComponentPhysics[ball].Velocity[1] +=
                    scene->ComponentPhysics[entity].Velocity[1] * 0.2f;
            }
        }
    }
}

static lcEntity_t
PaddleCreate(lcScene_t *scene,
             float x, float y,
             float *colour,
             lcAssetSprite_t *sprite)
{
    lcEntity_t result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                           x, y,
                           16.0f, 128.0f,
                           colour,
                           sprite);
    float min[] = { x - 16.0f, y - 128.0f };
    float max[] = { x + 16.0f, y + 128.0f };
    float vel[] = { 0.0f, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

    return result;
}

static lcEntity_t
BallCreate(lcScene_t *scene,
           float x, float y,
           float *colour,
           lcAssetSprite_t *sprite)
{
    lcEntity_t result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                           x, y,
                           12.0f, 12.0f,
                           colour,
                           sprite);
    float min[] = { x - 12.0f, y - 12.0f };
    float max[] = { x + 12.0f, y + 12.0f };
    float vel[] = { -BallSpeed, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

    return result;
}

lcInitConfig_t
lcClientConfig()
{
    lcInitConfig_t config;

    strcpy(config.WindowTitle, "Lucerna test!");
    config.WindowDimensions[0] = 1920;
    config.WindowDimensions[1] = 1017;
    config.CameraPosition[0] = 0.0f;
    config.CameraPosition[1] = 1.0f;
    config.VSyncEnabled = true;
    
    return config;
}

void
lcClientMain(int argc,
             char** argv)
{
    /* create the window */
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    g_winYBounds[0] = -540.0f;
    g_winYBounds[1] = 540.0f;
    g_winXBounds[0] = -960.0f;
    g_winXBounds[1] = 960.0f;
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, OnWindowResize);

    /* load assets */
    lcAssetSprite_t *backgroundTex = (lcAssetSprite_t *)lcLoadAsset("background");
    lcAssetSprite_t *ballTex = (lcAssetSprite_t *)lcLoadAsset("ball");
    lcAssetSprite_t *paddleTex = (lcAssetSprite_t *)lcLoadAsset("paddle");

    /* create a scene */
    lcScene_t *scene = lcSceneCreate();

    /* setup the renderer */
    lcShader_t shader = lcShaderCreate(LC_SHADER_PATH("SolidColour.vert"),
                                       LC_SHADER_PATH("SolidColour.frag"));

    lcRendererBindShader(shader);
    lcRendererBindScene(scene);

    /* create some entities */
    float bgCol[4] = { 1.0f, 0.5f, 0.8f, 1.0f };
    float fgCol[4] = { 1.0, 1.0f, 1.0f, 1.0f };

    lcEntity_t bg = lcEntityCreate(scene);
    ComponentRenderableAdd(scene,
                           bg,
                           0.0f,
                           0.0f,
                           1080.0f,
                           1080.0f,
                           bgCol,
                           backgroundTex);

    lcEntity_t playerPaddle = PaddleCreate(scene, -540.0f, 0.0f, fgCol, paddleTex);
    lcEntity_t computerPaddle = PaddleCreate(scene, +540.0f, 0.0f, fgCol, paddleTex);
    lcEntity_t ball = BallCreate(scene, 0.0f, 0.0f, fgCol, ballTex);

    /* setup physics */
    lcSubset_t physics = lcSubsetCreate();
    lcSubsetSetSignature(&physics,
                         COMPONENT_PHYSICS |
                         COMPONENT_RENDERABLE);
    lcSubsetRefresh(scene, &physics);

    /* main loop */
    double frameTimeInSeconds;
    uint64_t previousTime = lcClockGetTime();
    uint64_t time = lcClockGetTime();
    int count = 0;
    while (g_running)
    {
        previousTime = time;
        time = lcClockGetTime();
        frameTimeInSeconds = (time - previousTime) / 1000000.0;

        if (count++ == 60)
        {
            count = 0;
            LC_LOG_INFO("FPS: %f; Frame Time (in seconds): %f\n",
                        1.0 / frameTimeInSeconds,
                        frameTimeInSeconds);
        }

        UpdateBall(scene, &physics, ball);
        UpdateComputerPaddle(scene, ball, computerPaddle);
        UpdatePlayerPaddle(scene, playerPaddle);

        UpdatePhysics(&physics, scene, frameTimeInSeconds);

        lcRendererRenderToWindow();
        lcWindowUpdate();
    }

    /* cleanup */
    lcShaderDestroy(shader);
    lcSubsetDestroy(physics);
    free(scene);
}
