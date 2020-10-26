#include <math.h>
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
#define CourtBounds 640.0f

static uint8_t g_running = 1;
static float g_winYBounds[2]; /* top and bottom of screen so the ball can bounce */
lcAudioSource_t *g_sound;

static void
OnWindowClose(lcGenericMessage_t *message)
{
    g_running = 0;
}

static void
OnWindowResize(lcGenericMessage_t *message)
{
    lcWindowResizeMessage_t *resize;
    resize = (lcWindowResizeMessage_t *)message;

    g_winYBounds[0] = -((float)resize->Height / 2.0f);
    g_winYBounds[1] = +((float)resize->Height / 2.0f);
}

static void
UpdatePhysics(lcSubset_t *subset,
              lcScene_t *scene,
              double frameTime)
{
    int i;
    lcEntity_t entity;
    lcForEntityInSubset(i, entity, *subset)
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
    if (scene->ComponentPhysics[paddle].Minimum[1] + OponentBallRange >
        scene->ComponentPhysics[ball].Maximum[1])
    {
        if (scene->ComponentPhysics[paddle].Velocity[1] > -OponentSpeed)
        {
            scene->ComponentPhysics[paddle].Velocity[1] -= OponentAcceleration;
        }
    }
    else if (scene->ComponentPhysics[paddle].Maximum[1] - OponentBallRange <
             scene->ComponentPhysics[ball].Minimum[1])
    {
        if (scene->ComponentPhysics[paddle].Velocity[1] < OponentSpeed)
        {
            scene->ComponentPhysics[paddle].Velocity[1] += OponentAcceleration;
        }
    }
    else
    {
        scene->ComponentPhysics[paddle].Velocity[1] /= 1.5f;
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
    int i;
    float *ballMin;
    float *ballMax;
    lcEntity_t entity;

    ballMin = scene->ComponentPhysics[ball].Minimum;
    ballMax = scene->ComponentPhysics[ball].Maximum;

    if (ballMin[1] < g_winYBounds[0])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
    }
    else if (ballMax[1] > g_winYBounds[1])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
    }

    if (ballMin[0] < -CourtBounds)
    {
        LC_LOG_INFO("Computer wins... :(");
        exit(0);
    }
    else if (ballMax[0] > CourtBounds)
    {
        LC_LOG_INFO("Player wins! :)");
        exit(0);
    }

    lcForEntityInSubset(i, entity, *physicsSubset)
    {
        if (entity != ball)
        {
            float *min; 
            float *max; 

            min = scene->ComponentPhysics[entity].Minimum;
            max = scene->ComponentPhysics[entity].Maximum;

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
             lcSprite_t *sprite)
{
    lcEntity_t result;
    float min[2];
    float max[2];
    float vel[2];

    result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                           x, y,
                           128.0f, 128.0f,
                           colour,
                           sprite);

    min[0] = x - 16.0f;
    min[1] = y - 128.0f;

    max[0] = x + 16.0f;
    max[1] = y + 128.0f;

    vel[0] = 0.0f;
    vel[1] = 0.0f;

    lcAddComponentPhysics(scene, result, min, max, vel);

    return result;
}

static lcEntity_t
BallCreate(lcScene_t *scene,
           float x, float y,
           float *colour,
           lcSprite_t *sprite)
{
    lcEntity_t result;
    float min[2];
    float max[2];
    float vel[2];

    result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                           x, y,
                           32.0f, 32.0f,
                           colour,
                           sprite);

    min[0] = x - 32.0f;
    min[1] = y - 32.0f;

    max[0] = x + 32.0f;
    max[1] = y + 32.0f;

    vel[0] = -BallSpeed;
    vel[1] = 0.0f;

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
    config.VSyncEnabled = false;
    
    return config;
}

void
lcClientMain(int argc,
             char** argv)
{
    lcSprite_t *backgroundTex;
    lcSprite_t *paddleTex;
    lcSprite_t *ballTex;
    lcScene_t *scene;
    lcShader_t shader;
    float bgCol[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float fgCol[4] = { 1.0, 1.0f, 1.0f, 1.0f };
    lcEntity_t bg, playerPaddle, computerPaddle, ball;
    lcSubset_t physics;
    double frameTimeInSeconds;
    uint64_t previousTime, time;
    int count;

    /* create the window */
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    g_winYBounds[0] = -540.0f;
    g_winYBounds[1] = 540.0f;
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, OnWindowResize);

    /* load assets */
    backgroundTex = (lcSprite_t *)lcLoadAsset("bg", LC_ASSET_TYPE_SPRITE);
    ballTex = (lcSprite_t *)lcLoadAsset("ball", LC_ASSET_TYPE_SPRITE);
    paddleTex = (lcSprite_t *)lcLoadAsset("paddle", LC_ASSET_TYPE_SPRITE);
    shader = *((lcShader_t *)lcLoadAsset("SplitTone", LC_ASSET_TYPE_SHADER));
    g_sound = lcLoadAsset("testSound", LC_ASSET_TYPE_SOUND);

    /* create a scene */
    scene = lcSceneCreate();

    /* setup the renderer */
    lcRendererBindShader(shader);
    lcRendererBindScene(scene);

    /* create some entities */
    bg = lcEntityCreate(scene);
    ComponentRenderableAdd(scene,
                           bg,
                           0.0f,
                           0.0f,
                           1080.0f,
                           1080.0f,
                           bgCol,
                           backgroundTex);

    playerPaddle = PaddleCreate(scene, -540.0f, 0.0f, fgCol, paddleTex);
    computerPaddle = PaddleCreate(scene, +540.0f, 0.0f, fgCol, paddleTex);
    ball = BallCreate(scene, 0.0f, 0.0f, fgCol, ballTex);

    /* setup physics */
    physics = lcSubsetCreate();
    lcSubsetSetSignature(&physics,
                         COMPONENT_PHYSICS |
                         COMPONENT_RENDERABLE);
    lcSubsetRefresh(scene, &physics);

    lcAudioPlay(g_sound);

    /* main loop */
    previousTime = lcClockGetTime();
    time = lcClockGetTime();
    count = 0;
    while (g_running)
    {
        previousTime = time;
        time = lcClockGetTime();
        frameTimeInSeconds = (time - previousTime) / 1000000.0;

        if (count++ == 30000)
        {
            count = 0;
            LC_LOG_INFO("FPS: %f; Frame Time (in seconds): %f\n",
                        1.0 / frameTimeInSeconds, frameTimeInSeconds);
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
