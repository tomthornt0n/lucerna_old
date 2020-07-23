/* override the default max entities before including Lucerna.c */
#define LC_MAX_ENTITIES 1000
#include "Lucerna.c"

static uint8_t running = 1;

void OnWindowClose(lcMessage_t message)
{
    running = 0;
}

void UpdateMovingSqaures(lcSubset_t *subset)
{
    LC_SUBSET_LOOP(subset)
        lc_RenderableMove(entity, scene,
                          1.0f / (float)entity,
                          (float)entity / 10000.0f);
    LC_END_SUBSET_LOOP
}

int main(int argc, char** argv)
{
    /* initialise the message system */
    lc_MessageSystemInit();

    /* initialise logging */
    lc_LogInit();

    /* create the window */
    lc_WindowInit("Lucerna test!", 960, 540);
    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    lc_WindowSetVSync(1);

    /* create some scenes */
    lcScene_t scene1 = lc_SceneCreate();

    /* setup the renderer */
    float cameraPos[2] = {0.0f, 0.0f};
    lc_CameraInit("u_ViewProjectionMatrix",
                  cameraPos);

    lcShader_t shader = lc_ShaderCreate("Client/Assets/Shaders/SolidColour.vert",
                                        "Client/Assets/Shaders/SolidColour.frag");

    lc_RendererInit();

    lc_RendererBindShader(shader);
    lc_RendererBindScene(&scene1);

    /* create some entities */
    int size = 20;
    float cellSize = 32.0f;

    int x, y;
    for (x = 0; x < size; ++x)
    {
        for (y = 0; y < size; ++y)
        {
            lcEntity_t entity = lc_EntityCreate(&scene1);
            float col[] =
            {
                (float)x / ((float)size + 0.1f),
                (float)y / ((float)size + 0.1f),
                0.85f, 1.0f
            };

            lc_AddRenderable(entity, &scene1,
                             x * cellSize, y * cellSize,
                             cellSize, cellSize,
                             col);
        }
    }

    lcSubset_t *movingSquares = lc_SubsetCreate(&scene1);
    lc_SubsetSetSignature(movingSquares, LC__RENDERABLE);
    lc_SubsetRefresh(movingSquares);

    /* test PNG */
    lcImage_t img = lc_LoaderParsePNG("Client/Assets/Textures/test.png");
    free(img.Data);

    float cameraSpeed = 5.0f;

    /* main loop */
    while (running)
    {
        /* UpdateMovingSqaures(movingSquares); */

        if (lc_InputIsKeyPressed(LC_KEY_W))
        {
            float offset[] = { 0.0f, cameraSpeed };
            lc_CameraMove(offset);
        }
        else if (lc_InputIsKeyPressed(LC_KEY_S))
        {
            float offset[] = { 0.0f, -cameraSpeed };
            lc_CameraMove(offset);
        }

        if (lc_InputIsKeyPressed(LC_KEY_A))
        {
            float offset[] = { cameraSpeed, 0.0f };
            lc_CameraMove(offset);
        }
        else if (lc_InputIsKeyPressed(LC_KEY_D))
        {
            float offset[] = { -cameraSpeed, 0.0f };
            lc_CameraMove(offset);
        }

        lc_RendererRender();

        lc_WindowUpdate();
    }

    /* cleanup */
    lc_ShaderDestroy(shader);
    lc_SceneDestroy(&scene1);
    lc_CameraDestroy();
    lc_RendererDestroy();
    lc_WindowDestroy();
    lc_LogDestroy();
    lc_MessageSystemDestroy();
}
