#include "Lucerna.h"

static uint8_t running = 1;

static void
OnWindowClose(lcMessage_t message)
{
    running = 0;
}

static void
UpdateMovingSqaures(lcSubset_t *subset)
{
    LC_SUBSET_LOOP(subset)
        lcRenderableMove(entity, scene,
                          1.0f / (float)entity,
                          (float)entity / 10000.0f);
    LC_END_SUBSET_LOOP
}

void
lcClientMain(int argc,
             char** argv)
{
    /* create the window */
    lcWindowInit("Lucerna test!", 960, 540);
    lcMessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    lcWindowSetVSync(1);

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
    int size = 40;
    float cellSize = 32.0f;

    int x, y;
    for (x = 0; x < size; ++x)
    {
        for (y = 0; y < size; ++y)
        {
            lcEntity_t entity = lcEntityCreate(&scene1);
            float col[] =
            {
                (float)x / ((float)size + 0.1f),
                (float)y / ((float)size + 0.1f),
                0.85f, 1.0f
            };

            lcAddRenderable(entity, &scene1,
                             x * cellSize, y * cellSize,
                             cellSize, cellSize,
                             col);
        }
    }

    lcSubset_t *movingSquares = lcSubsetCreate(&scene1);
    lcSubsetSetSignature(movingSquares, LC__RENDERABLE);
    lcSubsetRefresh(movingSquares);

    /* test PNG */
    lcImage_t img = lcLoaderParsePNG("Client/Assets/Textures/test.png");
    free(img.Data);

    float cameraSpeed = 5.0f;

    /* main loop */
    while (running)
    {
        /* UpdateMovingSqaures(movingSquares); */

        if (lcInputIsKeyPressed(LC_KEY_W))
        {
            float offset[] = { 0.0f, cameraSpeed };
            lcCameraMove(offset);
        }
        else if (lcInputIsKeyPressed(LC_KEY_S))
        {
            float offset[] = { 0.0f, -cameraSpeed };
            lcCameraMove(offset);
        }

        if (lcInputIsKeyPressed(LC_KEY_A))
        {
            float offset[] = { cameraSpeed, 0.0f };
            lcCameraMove(offset);
        }
        else if (lcInputIsKeyPressed(LC_KEY_D))
        {
            float offset[] = { -cameraSpeed, 0.0f };
            lcCameraMove(offset);
        }

        lcRendererRender();

        lcWindowUpdate();
    }

    /* cleanup */
    lcShaderDestroy(shader);
    lcSceneDestroy(&scene1);
    lcRendererDestroy();
    lcCameraDestroy();
    lcWindowDestroy();
}
