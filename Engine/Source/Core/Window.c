/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
  Notes   : Uses GLFW for now, will hopefully write own platform layer at some
            point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static GLFWwindow *lcWindow;

static void
GLFWErrorCallback(int error, const char *description)
{
    LC_ASSERT(0, "GLFW ERROR(%d): %s", error, description);
}

static void
WindowCloseFunction(GLFWwindow *window)
{
    lcMessageEmit(lcWindowCloseMessageCreate());
}

static void
WindowResizeFunction(GLFWwindow *window,
                     int width, int height)
{
    lcMessageEmit(lcWindowResizeMessageCreate(width, height));
}

static void
WindowKeyFunction(GLFWwindow *window,
                  int key, int scanCode, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            lcMessageEmit(lcKeyPressMessageCreate(key, 0));
            break;
        }
        case GLFW_RELEASE:
        {
            lcMessageEmit(lcKeyReleaseMessageCreate(key));
            break;
        }
        case GLFW_REPEAT:
        {
            lcMessageEmit(lcKeyPressMessageCreate(key, 1));
            break;
        }
    }
}

static void
WindowMouseButtonFunction(GLFWwindow *window,
                          int key, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            lcMessageEmit(lcMouseButtonPressMessageCreate(key));
            break;
        }
        case GLFW_RELEASE:
        {
            lcMessageEmit(lcMouseButtonReleaseMessageCreate(key));
            break;
        }
    }
}

static void
WindowMouseScrollFunction(GLFWwindow *window,
                          double xOffset, double yOffset)
{
    lcMessageEmit(lcMouseScrollMessageCreate(xOffset, yOffset));
}

void
lcWindowInit(const char *title,
              int width, int height)
{
    int success = glfwInit();
    LC_ASSERT(success, "Could not initialize GLFW");

    glfwSetErrorCallback(GLFWErrorCallback);

    lcWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    LC_ASSERT(lcWindow, "GLFW ERROR: Window creation failed!");
    
    glfwMakeContextCurrent(lcWindow);

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LC_ASSERT(status, "ERROR: failed to initialise glad!");

    glfwSetWindowCloseCallback(lcWindow, WindowCloseFunction);
    glfwSetFramebufferSizeCallback(lcWindow, WindowResizeFunction);    
    glfwSetKeyCallback(lcWindow, WindowKeyFunction);    
    glfwSetMouseButtonCallback(lcWindow, WindowMouseButtonFunction);
    glfwSetScrollCallback(lcWindow, WindowMouseScrollFunction);
}

void
lcWindowUpdate(void)
{
    glfwPollEvents();
    glfwSwapBuffers(lcWindow);
}

void
lcWindowSetVSync(bool enabled)
{
    glfwSwapInterval(enabled);
}

void
lcWindowDestroy(void)
{
    glfwDestroyWindow(lcWindow);
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

