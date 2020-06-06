#include <stdbool.h>

#include "Lucerna.h"

static bool running = true;

static lc_Window* window;

const float CAMERA_MOVE_SPEED = 128.0f;

void OnWindowClose()
{
    running = false;
}

void OnWindowResize(int width, int height)
{
    LC_TRACE("Window resize: %i, %i", width, height);
}

int main()
{
    //create the window
    window = lc_CreateWindow("Lucerna test!", 960, 540);
    window->Data.WindowClosedCallback = OnWindowClose;
    window->Data.WindowResizeCallback = OnWindowResize;

    //initialise logging
    lc_InitLog();

    //main loop
    while (running)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        lc_UpdateWindow(window);
    }

    //cleanup
    lc_DestroyWindow(window);
    lc_DestroyLog();
}
