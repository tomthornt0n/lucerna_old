/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

struct 
{
    GLFWwindow *_NativeWindow;

    const char *Title;
    uint32_t Width, Height;
    uint8_t VSync;
} lc_Window;

void
GLFWErrorCallback(int error, const char *description)
{
    LC_ASSERT(0, "GLFW ERROR(%d): %s", error, description);
}

void
WindowCloseFunction(GLFWwindow *window)
{
    lc_MessageEmit(lc_WindowCloseMessageCreate());
}

void
WindowResizeFunction(GLFWwindow *window,
                     int width, int height)
{
    lc_Window.Width = width;
    lc_Window.Height = height;

    lc_MessageEmit(lc_WindowResizeMessageCreate(width, height));
}

void
WindowKeyFunction(GLFWwindow *window,
                  int key, int scanCode, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            lc_MessageEmit(lc_KeyPressMessageCreate(key, 0));
            break;
        }
        case GLFW_RELEASE:
        {
            lc_MessageEmit(lc_KeyReleaseMessageCreate(key));
            break;
        }
        case GLFW_REPEAT:
        {
            lc_MessageEmit(lc_KeyPressMessageCreate(key, 1));
            break;
        }
    }
}

void
WindowMouseButtonFunction(GLFWwindow *window,
                          int key, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            lc_MessageEmit(lc_MouseButtonPressMessageCreate(key));
            break;
        }
        case GLFW_RELEASE:
        {
            lc_MessageEmit(lc_MouseButtonReleaseMessageCreate(key));
            break;
        }
    }
}

void
WindowMouseScrollFunction(GLFWwindow *window,
                          double xOffset, double yOffset)
{
    lc_MessageEmit(lc_MouseScrollMessageCreate(xOffset, yOffset));
}

void
lc_WindowInit(const char *title,
              int width, int height)
{
    lc_Window.Title = title;

    lc_Window.Width = width;
    lc_Window.Height = height;

    int success = glfwInit();
    LC_ASSERT(success, "Could not initialize GLFW");

    glfwSetErrorCallback(GLFWErrorCallback);

    lc_Window._NativeWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    LC_ASSERT(lc_Window._NativeWindow, "GLFW ERROR: Window creation failed!");
    
    glfwMakeContextCurrent(lc_Window._NativeWindow);

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LC_ASSERT(status, "ERROR: failed to initialise glad!");

    glfwSetWindowCloseCallback(lc_Window._NativeWindow, WindowCloseFunction);
    glfwSetWindowSizeCallback(lc_Window._NativeWindow, WindowResizeFunction);    
    glfwSetKeyCallback(lc_Window._NativeWindow, WindowKeyFunction);    
    glfwSetMouseButtonCallback(lc_Window._NativeWindow, WindowMouseButtonFunction);
    glfwSetScrollCallback(lc_Window._NativeWindow, WindowMouseScrollFunction);
}

void
lc_WindowUpdate(void)
{
    glfwPollEvents();

    /* NOTE(tbt): don't swap buffers when the window is minimised */
    if(lc_Window.Width > 0 && lc_Window.Height > 0)
    {
        glfwSwapBuffers(lc_Window._NativeWindow);
    }
}

void
lc_WindowSetVSync(uint8_t enabled)
{
    lc_Window.VSync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

void
lc_WindowDestroy(void)
{
    glfwDestroyWindow(lc_Window._NativeWindow);
}


/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

