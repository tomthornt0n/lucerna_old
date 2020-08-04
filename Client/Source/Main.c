#include <math.h>

#include "Lucerna.h"

#include "GLFW/glfw3.h"

#define PlayerAcceleration 20.0f
#define PlayerSpeed 250.0f
#define OponentSpeed 200.0f
#define OponentBallRange 24.0f
#define BallSpeed 200.0f

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
              double frameTime)
{
    LC_SUBSET_LOOP(subset)
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

        lcRenderableMove(entity, scene,
                         scene->ComponentPhysics[entity].Velocity[0] *
                         frameTime,
                         scene->ComponentPhysics[entity].Velocity[1] *
                         frameTime);
    LC_END_SUBSET_LOOP
}

static void
UpdateComputerPaddle(lcScene_t *scene,
                     lcEntity_t ball,
                     lcEntity_t paddle)
{
    if (scene->ComponentPhysics[paddle].Minimum[1] >
        scene->ComponentPhysics[ball].Maximum[1] + OponentBallRange)
    {
        scene->ComponentPhysics[paddle].Velocity[1] = -OponentBallRange;
    }
    else if (scene->ComponentPhysics[paddle].Maximum[1] <
             scene->ComponentPhysics[ball].Minimum[1] - 32.0f)
    {
        scene->ComponentPhysics[paddle].Velocity[1] = OponentSpeed;
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

    LC_SUBSET_LOOP(physicsSubset)
        if (entity != ball)
        {
            float *min = scene->ComponentPhysics[entity].Minimum;
            float *max = scene->ComponentPhysics[entity].Maximum;

            if (!(max[0] < ballMin[0] || min[0] > ballMax[0]) &&
                !(max[1] < ballMin[1] || min[1] > ballMax[1]))
            {
                scene->ComponentPhysics[ball].Velocity[0] *= -1.0f;
                scene->ComponentPhysics[ball].Velocity[1] *= -1.0f;
                scene->ComponentPhysics[ball].Velocity[1] +=
                    scene->ComponentPhysics[entity].Velocity[1] * 0.2f;
            }
        }
    LC_END_SUBSET_LOOP
}

static lcEntity_t
PaddleCreate(lcScene_t *scene,
             float x, float y,
             float *colour)
{
    lcEntity_t result = lcEntityCreate(scene);
    lcAddComponentRenderable(scene, result,
                             x, y,
                             16.0f, 128.0f,
                             colour);
    float min[] = { x - 16.0f, y - 128.0f };
    float max[] = { x + 16.0f, y + 128.0f };
    float vel[] = { 0.0f, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

    return result;
}

static lcEntity_t
BallCreate(lcScene_t *scene,
           float x, float y,
           float *colour)
{
    lcEntity_t result = lcEntityCreate(scene);
    lcAddComponentRenderable(scene, result,
                             x, y,
                             12.0f, 12.0f,
                             colour);
    float min[] = { x - 12.0f, y - 12.0f };
    float max[] = { x + 12.0f, y + 12.0f };
    float vel[] = { -BallSpeed, 0.0f };
    lcAddComponentPhysics(scene, result, min, max, vel);

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

    /* create some scenes */
    lcScene_t scene1 = lcSceneCreate();

    /* setup the renderer */
    float cameraPos[2] = {0.0f, 0.0f};
    lcCameraInit("u_ViewProjectionMatrix",
                  cameraPos);

    lcShader_t shader = lcShaderCreate("Client/Assets/Shaders/SolidColour.vert",
                                       "Client/Assets/Shaders/SolidColour.frag");
    lcRendererInit();

    lcRendererBindShader(shader);
    lcRendererBindScene(&scene1);

    /* create some entities */
    float bgCol[4] = { 0.110f, 0.145f, 0.200f, 1.000f };
    float fgCol[4] = { 0.961f, 0.965f, 0.953f, 1.000f };

    lcEntity_t bg = lcEntityCreate(&scene1);
    lcAddComponentRenderable(&scene1, bg, 0.0f, 0.0f, 2560.0f, 1440.0f, bgCol);

    lcEntity_t playerPaddle = PaddleCreate(&scene1, -540.0f, 0.0f, fgCol);
    lcEntity_t computerPaddle = PaddleCreate(&scene1, +540.0f, 0.0f, fgCol);
    lcEntity_t ball = BallCreate(&scene1, 0.0f, 0.0f, fgCol);

    /* setup physics */
    lcSubset_t *physics = lcSubsetCreate(&scene1);
    lcSubsetSetSignature(physics,
                         COMPONENT_PHYSICS |
                         COMPONENT_RENDERABLE);
    lcSubsetRefresh(physics);

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
            fprintf(stderr, "FPS: %f\n", 1.0 / frameTime);
        }

        UpdateBall(&scene1, physics, ball);
        UpdateComputerPaddle(&scene1, ball, computerPaddle);
        UpdatePlayerPaddle(&scene1, playerPaddle);

        UpdatePhysics(physics, frameTime);

        lcRendererRender();
        lcWindowUpdate();
    }

    /* cleanup */
    lcShaderDestroy(shader);
    lcSubsetDestroy(physics);
    lcSceneDestroy(&scene1);
    lcRendererDestroy();
    lcCameraDestroy();
    lcWindowDestroy();
}
