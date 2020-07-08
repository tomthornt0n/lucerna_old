#include "Lucerna.h"

static uint8_t running = 1;

lcScene_t scene1;
lcScene_t scene2;

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

void BindScene1(lcMessage_t message)
{
    if (message.KeyPress.KeyCode == LC_KEY_1 &&
        message.KeyPress.Repeat == 0)
    {
        lc_RendererBindScene(&scene1);
    }
}

void BindScene2(lcMessage_t message)
{
    if (message.KeyPress.KeyCode == LC_KEY_2 &&
        message.KeyPress.Repeat == 0)
    {
        lc_RendererBindScene(&scene2);
    }
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

    /* create some scenes */
    lc_SceneCreate(&scene1);
    lc_SceneCreate(&scene2);

    lc_MessageBind(LC_MESSAGE_TYPE_KEY_PRESS, BindScene1);
    lc_MessageBind(LC_MESSAGE_TYPE_KEY_PRESS, BindScene2);

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
    lcEntity_t entity1 = lc_EntityCreate(&scene1);
    lcEntity_t entity2 = lc_EntityCreate(&scene2); 

    /* add the renderable component */
    float entity1Colour[4] = { 0.55f, 0.91f, 0.99f, 1.0f };
    lc_AddRenderable(entity1, &scene1,
                     0.0f,  0.0f,
                     64.0f, 64.0f,
                     entity1Colour);

    float entity2Colour[4] = { 0.74f, 0.58f, 0.98f, 1.0f };
    lc_AddRenderable(entity2, &scene2,
                     0.0f, 5.0f,
                     64.0f, 64.0f,
                     entity2Colour);


    /* main loop */
    while (running)
    {
        if (lc_InputIsKeyPressed(LC_KEY_W))
            lc_RenderableMove(entity1, &scene1,  0.0f, -1.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_S))
            lc_RenderableMove(entity1, &scene1, 0.0f, 1.0f);

        if (lc_InputIsKeyPressed(LC_KEY_A))
            lc_RenderableMove(entity1, &scene1, -1.0f, 0.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_D))
            lc_RenderableMove(entity1, &scene1, 1.0f, 0.0f);

        if (lc_InputIsKeyPressed(LC_KEY_UP))
            lc_RenderableMove(entity2, &scene2, 0.0f, -1.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_DOWN))
            lc_RenderableMove(entity2, &scene2, 0.0f, 1.0f);

        if (lc_InputIsKeyPressed(LC_KEY_LEFT))
            lc_RenderableMove(entity2, &scene2, -1.0f, 0.0f);
        else if (lc_InputIsKeyPressed(LC_KEY_RIGHT))
            lc_RenderableMove(entity2, &scene2, 1.0f, 0.0f);

        /*
        if (lc_InputIsKeyPressed(LC_KEY_1))
        {
            LC_LOG_DEBUG("Switching to scene 1");
            lc_RendererBindScene(&scene1);
        }

        if (lc_InputIsKeyPressed(LC_KEY_2))
        {
            LC_LOG_DEBUG("Switching to scene 2");
            lc_RendererBindScene(&scene1);
        }
        */

        lc_RendererRender();

        lc_WindowUpdate();
    }

    /* cleanup */
    lc_ShaderDestroy(shader);
    lc_CameraDestroy();
    lc_RendererDestroy();
    lc_WindowDestroy();
    lc_LogDestroy();
    lc_MessageSystemDestroy();
}
