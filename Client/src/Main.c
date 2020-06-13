#include "Lucerna.h"

static uint8_t running = 1;

static lcWindow_t window;

const float CAMERA_MOVE_SPEED = 128.0f;

void OnWindowClose(void)
{
    running = 0;
}

void OnWindowResize(int width, int height)
{
    LC_TRACE("Window resize: %i, %i", width, height);
}

int main(int argc, char** argv)
{
    /* initialise logging */
    lc_InitLog();

    /* create the window */
    lc_CreateWindow(&window, "Lucerna test!", 960, 540);
    window.Data.WindowClosedCallback = OnWindowClose;
    window.Data.WindowResizeCallback = OnWindowResize;
    lc_SetWindowVSync(&window, 1);

    /* create a scene */
    lcScene_t scene;
    lc_CreateScene(&scene);

    /* setup the renderer */
    lcRenderer_t renderer;
    lcShader_t shader = lc_CreateShader("Client/assets/shaders/solidColour.vert",
                                        "Client/assets/shaders/solidColour.frag");
    lc_CreateRenderer(&renderer, &scene, shader);

    /* camera */
    lcCamera_t camera;
    float cameraPos[2] = {0.0f, 0.0f};
    lc_CreateCamera(&camera,
                    &renderer,
                    "u_ViewProjectionMatrix",
                    960, 540,
                    cameraPos);

    /* create some entities */
    lcEntity_t entity2 = lc_CreateEntity(&scene); 
    lcEntity_t entity1 = lc_CreateEntity(&scene);

    /* add the renderable component */
    float entity1Colour[4] = { 0.55f, 0.91f, 0.99f, 1.0f };
    lc_AddRenderable(&renderer,
                     entity1,
                     200.0f, 200.0f,
                     64.0f, 64.0f,
                     entity1Colour);

    float entity2Colour[4] = { 0.74f, 0.58f, 0.98f, 1.0f };
    lc_AddRenderable(&renderer,
                     entity2,
                     0.0f, 5.0f,
                     64.0f, 64.0f,
                     entity2Colour);
       
    /* test vector functions */
    float vector1[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
    float vector2[4] = { 0.5f, 1.0f, 1.5f, 2.0f };
    float result[4];
    lc_Vector4Divide(result, vector1, vector2);
    int i;
    for (i = 0; i < 4; ++i)
        LC_LOG("%f", result[i]);

    /* main loop */
    while (running)
    {
        if (lc_IsKeyPressed(&window, LC_KEY_W))
            lc_MoveRenderable(&renderer, entity1, 0.0f, -1.0f);
        if (lc_IsKeyPressed(&window, LC_KEY_S))
            lc_MoveRenderable(&renderer, entity1, 0.0f, 1.0f);

        if (lc_IsKeyPressed(&window, LC_KEY_A))
            lc_MoveRenderable(&renderer, entity1, -1.0f, 0.0f);
        if (lc_IsKeyPressed(&window, LC_KEY_D))
            lc_MoveRenderable(&renderer, entity1, 1.0f, 0.0f);

        if (lc_IsKeyPressed(&window, LC_KEY_UP))
            lc_MoveRenderable(&renderer, entity2, 0.0f, -1.0f);
        else if (lc_IsKeyPressed(&window, LC_KEY_DOWN))
            lc_MoveRenderable(&renderer, entity2, 0.0f, 1.0f);

        if (lc_IsKeyPressed(&window, LC_KEY_LEFT))
            lc_MoveRenderable(&renderer, entity2, -1.0f, 0.0f);
        else if (lc_IsKeyPressed(&window, LC_KEY_RIGHT))
            lc_MoveRenderable(&renderer, entity2, 1.0f, 0.0f);

        lc_Render(&renderer);

        lc_UpdateWindow(&window);
    }

    /* cleanup */
    lc_DestroyRenderer(&renderer);
    lc_DestroyWindow(&window);
    lc_DestroyLog();
}
