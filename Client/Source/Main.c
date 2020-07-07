#include "Lucerna.h"

static uint8_t running = 1;

void OnWindowClose(lcMessage_t message)
{
    running = 0;
}

void OnWindowResize(lcMessage_t message)
{
    LC_LOG_DEBUG("Window resize event: %u x %u",
                 message.WindowResize.Width,
                 message.WindowResize.Height);
}

int main(int argc, char** argv)
{
    /* initialise the message system */
    lc_MessageSystemInit();

    /* initialise logging */
    lc_LogInit();

    /* create the window */
    lc_WindowInit("Lucerna test!", 960, 540);
    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_RESIZE, OnWindowResize);
    lc_MessageBind(LC_MESSAGE_TYPE_WINDOW_CLOSE, OnWindowClose);
    lc_WindowSetVSync(1);

    /* create a scene */
    lcScene_t scene;
    lc_SceneCreate(&scene);

    /* setup the renderer */
    float cameraPos[2] = {0.0f, 0.0f};
    lc_CameraInit("u_ViewProjectionMatrix",
                  cameraPos);

    lcShader_t shader = lc_ShaderCreate("Client/Assets/Shaders/SolidColour.vert",
                                        "Client/Assets/Shaders/SolidColour.frag");

    lc_RendererInit(&scene, shader);

    /* create some entities */
    lcEntity_t entity2 = lc_EntityCreate(&scene); 
    lcEntity_t entity1 = lc_EntityCreate(&scene);

    /* add the renderable component */
    float entity1Colour[4] = { 0.55f, 0.91f, 0.99f, 1.0f };
    lc_AddRenderable(entity1, &scene,
                     200.0f, 200.0f,
                     64.0f, 64.0f,
                     entity1Colour);

    float entity2Colour[4] = { 0.74f, 0.58f, 0.98f, 1.0f };
    lc_AddRenderable(entity2, &scene,
                     0.0f, 5.0f,
                     64.0f, 64.0f,
                     entity2Colour);


    /* main loop */
    while (running)
    {
        if (lc_InputIsKeyPressed(LC_KEY_W))
            lc_RenderableMove(entity1, &scene,  0.0f, -1.0f);
        if (lc_InputIsKeyPressed(LC_KEY_S))
            lc_RenderableMove(entity1, &scene, 0.0f, 1.0f);

        if (lc_InputIsKeyPressed(LC_KEY_A))
            lc_RenderableMove(entity1, &scene, -1.0f, 0.0f);
        if (lc_InputIsKeyPressed(LC_KEY_D))
            lc_RenderableMove(entity1, &scene, 1.0f, 0.0f);

        if (lc_InputIsKeyPressed(LC_KEY_UP))
            lc_RenderableMove(entity2, &scene, 0.0f, -1.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_DOWN))
            lc_RenderableMove(entity2, &scene, 0.0f, 1.0f);

        if (lc_InputIsKeyPressed(LC_KEY_LEFT))
            lc_RenderableMove(entity2, &scene, -1.0f, 0.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_RIGHT))
            lc_RenderableMove(entity2, &scene, 1.0f, 0.0f);

        lc_RendererRender(&scene, shader);

        lc_WindowUpdate();
    }

    /* cleanup */
    lc_ShaderDestroy(shader);
    lc_RendererDestroy();
    lc_WindowDestroy();
    lc_LogDestroy();
    lc_MessageSystemDestroy();
}
