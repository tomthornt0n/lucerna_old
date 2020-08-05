#include <math.h>

#include "Lucerna.h"

#include "GLFW/glfw3.h"

#define PlayerAcceleration 40.0f
#define PlayerSpeed 450.0f
#define OponentSpeed 450.0f
#define OponentAcceleration 40.0f
#define OponentBallRange 0.0f
#define BallSpeed 500.0f

static uint8_t running = 1;
static float winBounds[2]; /* top and bottom of screen so the ball can bounce */

static void
OnWindowClose(lcMessage_t message)
{
    running = 0;
}

static void
OnWindowResize(lcMessage_t message)
{
    winBounds[0] = -((float)message.WindowResize.Height / 2.0f);
    winBounds[1] = +((float)message.WindowResize.Height / 2.0f);
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
    }
}

static void
UpdatePlayerPaddle(lcScene_t *scene,
                   lcEntity_t paddle)
{
    if (lcInputIsKeyPressed(LC_KEY_S) &&
        scene->ComponentPhysics[paddle].Velocity[1] < PlayerSpeed)
    {
        scene->ComponentPhysics[paddle].Velocity[1] += PlayerAcceleration;
    }
    else if (lcInputIsKeyPressed(LC_KEY_W) &&
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

    if (ballMin[1] < winBounds[0])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
    }
    else if (ballMax[1] > winBounds[1])
    {
        scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
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
             float *colour)
{
    lcEntity_t result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                             x, y,
                             16.0f, 128.0f,
                             colour);
    float min[] = { x - 16.0f, y - 128.0f };
    float max[] = { x + 16.0f, y + 128.0f };
    float vel[] = { 0.0f, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

    LC_LOG_DEBUG("Paddle: %u", result);

    return result;
}

static lcEntity_t
BallCreate(lcScene_t *scene,
           float x, float y,
           float *colour)
{
    lcEntity_t result = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, result,
                             x, y,
                             12.0f, 12.0f,
                             colour);
    float min[] = { x - 12.0f, y - 12.0f };
    float max[] = { x + 12.0f, y + 12.0f };
    float vel[] = { -BallSpeed, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

    LC_LOG_DEBUG("Ball: %u", result);

    return result;
}

void
lcClientMain(int argc,
             char** argv)
{
    /* create the window */
    lcWindowInit("Lucerna test!", 960, 540);
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    lcWindowSetVSync(false);
    winBounds[0] = -480.0f;
    winBounds[1] = 480.0f;
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, OnWindowResize);

    /* create a scene */
    lcScene_t *scene = lcSceneCreate();

    /* setup the renderer */
    float cameraPos[2] = {0.0f, 0.0f};
    lcCameraInit("u_ViewProjectionMatrix",
                  cameraPos);

    lcShader_t shader = lcShaderCreate("Client/Assets/Shaders/SolidColour.vert",
                                       "Client/Assets/Shaders/SolidColour.frag");
    lcRendererInit();

    lcRendererBindShader(shader);
    lcRendererBindScene(scene);

    /* create some entities */
    float bgCol[4] = { 0.110f, 0.145f, 0.200f, 1.000f };
    float fgCol[4] = { 0.961f, 0.965f, 0.953f, 1.000f };

    lcEntity_t bg = lcEntityCreate(scene);
    ComponentRenderableAdd(scene, bg, 0.0f, 0.0f, 2560.0f, 1440.0f, bgCol);

    lcEntity_t playerPaddle = PaddleCreate(scene, -540.0f, 0.0f, fgCol);
    lcEntity_t computerPaddle = PaddleCreate(scene, +540.0f, 0.0f, fgCol);
    lcEntity_t ball = BallCreate(scene, 0.0f, 0.0f, fgCol);

    /* setup physics */
    lcSubset_t physics = lcSubsetCreate();
    lcSubsetSetSignature(&physics,
                         COMPONENT_PHYSICS |
                         COMPONENT_RENDERABLE);
    lcSubsetRefresh(scene, &physics);

    /* main loop */
    double frameTime;
    double previousTime = glfwGetTime();
    double time = glfwGetTime();
    int count = 0;
    while (running)
    {
        previousTime = time;
        time = glfwGetTime();
        frameTime = time - previousTime;

        if (count++ == 10000)
        {
            count = 0;
            fprintf(stderr, "FPS: %f\nFrame Time: %f\n\n",
                    1.0 / frameTime,
                    frameTime);
        }

        UpdateBall(scene, &physics, ball);
        UpdateComputerPaddle(scene, ball, computerPaddle);
        UpdatePlayerPaddle(scene, playerPaddle);

        UpdatePhysics(&physics, scene, frameTime);

        lcRendererRenderToWindow();
        lcWindowUpdate();
    }

    /* cleanup */
    lcShaderDestroy(shader);
    lcSubsetDestroy(physics);
    free(scene);
    lcRendererDestroy();
    lcCameraDestroy();
    lcWindowDestroy();
}
