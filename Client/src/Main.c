#include <stdbool.h>

#include "Lucerna.h"

static bool running = true;

static lc_Window* window;

const float CAMERA_MOVE_SPEED = 128.0f;

void OnWindowClose(void)
{
    running = false;
}

void OnWindowResize(int width, int height)
{
    LC_TRACE("Window resize: %i, %i", width, height);
}

int main(int argc, char** argv)
{
    //initialise logging
    lc_InitLog();

    //create the window
    window = lc_CreateWindow("Lucerna test!", 960, 540);
    window->Data.WindowClosedCallback = OnWindowClose;
    window->Data.WindowResizeCallback = OnWindowResize;
    lc_SetWindowVSync(window, false);

    //create a scene
    lc_Scene* scene = lc_CreateScene();

    //setup the renderer
    lc_InitRenderer(scene);
    lc_Shader shader = lc_CreateShader("Client/assets/shaders/solidColour.vert", "Client/assets/shaders/solidColour.frag");

    //create some entities
    lc_Entity entity1  = lc_CreateEntity(scene);
    lc_Entity entity2  = lc_CreateEntity(scene);
    lc_Entity entity3  = lc_CreateEntity(scene);

    //add the renderable component
    float entity1Colour[4] = { 0.55f, 0.91f, 0.99f, 1.0f };
    lc_AddRenderable(scene, entity1, -0.4f, -0.4f, 0.25f, 0.25f, entity1Colour);
       
    float entity2Colour[4] = { 0.74f, 0.58f, 0.98f, 1.0f };
    lc_AddRenderable(scene, entity2, 0.4f, 0.4f, 0.25f, 0.25f, entity2Colour);

    float entity3Colour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    lc_AddRenderable(scene, entity3, 0.0f, 0.0f, 0.4f, 0.4f, entity3Colour);


    //main loop
    while (running)
    {
        lc_Render(shader);

        lc_UpdateWindow(window);
    }

    //cleanup
    lc_DestroyRenderer();
    lc_DestroyWindow(window);
    lc_DestroyLog();
}
