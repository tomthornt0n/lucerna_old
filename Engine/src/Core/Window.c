//stored in the GLFWwindow's user pointer area, so can be acessed from the GLFWwindow, without need of the lc_Window
typedef struct
{
    const char* Title;
    int Width, Height;
    bool VSync;

    //event callbacks
    void (*WindowClosedCallback)();
    void (*WindowResizeCallback)(int width, int height);
    void (*KeyPressedCallback)(int keyCode, bool repeat);
    void (*KeyReleasedCallback)(int keycode);
    void (*KeyTypedCallback)(int keyCode);
    void (*MouseButtonPressedCallback)(int keyCode);
    void (*MouseButtonReleasedCallback)(int keyCode);
    void (*MouseScrolledCallback)(double xOffset, double yOffset);
} lc_WindowData;

typedef struct
{
    GLFWwindow* NativeWindow;
    lc_WindowData Data; 
} lc_Window;


static bool s_GLFWInitialized                              = false;

static void GLFWErrorCallback(int error, const char* description)
{
    LC_ASSERT(false, "GLFW ERROR(%d): %s", error, description);
}

//default user event callbacks
static void DefaultWindowCloseCallback(void) {}
static void DefaultWindowResizeCallback(int width, int height) {}
static void DefaultKeyPressedCallback(int keyCode, bool repeat) {}
static void DefaultKeyReleasedCallback(int keycode) {}
static void DefaultKeyTypedCallback(int keyCode) {}
static void DefaultMouseButtonPressedCallback(int keyCode) {}
static void DefaultMouseButtonReleasedCallback(int keyCode) {}
static void DefaultMouseScrolledCallback(double xOffset, double yOffset) {}

//window event callback functions
static void WindowCloseFunction(GLFWwindow* window)
{
    lc_WindowData* data                                    = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->WindowClosedCallback();
};
static void WindowResizeFunction(GLFWwindow* window, int width, int height)
{
    lc_WindowData* data                                    = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->Width                                            = width;
    data->Height                                           = height;

    data->WindowResizeCallback(width, height);
};
static void WindowKeyFunction(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    lc_WindowData* data                                    = (lc_WindowData*)glfwGetWindowUserPointer(window);
    switch (action)
    {
        case GLFW_PRESS:
        {
            data->KeyPressedCallback(key, false);
            break;
        }
        case GLFW_RELEASE:
        {
            data->KeyReleasedCallback(key);
            break;
        }
        case GLFW_REPEAT:
        {
            data->KeyPressedCallback(key, true);
            break;
        }
    }
};
static void WindowCharFunction(GLFWwindow* window, unsigned int key)
{
    lc_WindowData* data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->KeyTypedCallback(key);
};
static void WindowMouseButtonFunction(GLFWwindow* window, int key, int action, int mods)
{
    lc_WindowData* data                                    = (lc_WindowData*)glfwGetWindowUserPointer(window);
    switch (action)
    {
        case GLFW_PRESS:
        {
            data->MouseButtonPressedCallback(key);
            break;
        }
        case GLFW_RELEASE:
        {
            data->MouseButtonReleasedCallback(key);
            break;
        }
    }
};
static void WindowMouseScrollFunction(GLFWwindow* window, double xOffset, double yOffset)
{
    lc_WindowData* data                                    = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->MouseScrolledCallback(xOffset, yOffset);
};

lc_Window* lc_CreateWindow(const char* title, int width, int height)
{
    lc_Window* window                                      = malloc(sizeof(lc_Window));

    window->Data.Title                                     = title;
    window->Data.Width                                     = width;
    window->Data.Height                                    = height;

    window->Data.WindowClosedCallback                      = DefaultWindowCloseCallback;
    window->Data.WindowResizeCallback                      = DefaultWindowResizeCallback;
    window->Data.KeyPressedCallback                        = DefaultKeyPressedCallback;
    window->Data.KeyReleasedCallback                       = DefaultKeyReleasedCallback;
    window->Data.KeyTypedCallback                          = DefaultKeyTypedCallback;
    window->Data.MouseButtonPressedCallback                = DefaultMouseButtonPressedCallback;
    window->Data.MouseButtonReleasedCallback               = DefaultMouseButtonReleasedCallback;
    window->Data.MouseScrolledCallback                     = DefaultMouseScrolledCallback;


    if (!s_GLFWInitialized)
    {
        int success                                        = glfwInit();
        LC_ASSERT(success, "Could not initialize GLFW");

        glfwSetErrorCallback(GLFWErrorCallback);

        s_GLFWInitialized                                  = true;
    }

    window->NativeWindow                                   = glfwCreateWindow(width, height, title, NULL, NULL);
    LC_ASSERT(window->NativeWindow, "GLFW ERROR: Window creation failed!");
    
    glfwMakeContextCurrent(window->NativeWindow);

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LC_ASSERT(status, "ERROR: failed to initialise glad!");

    glfwSetWindowUserPointer(window->NativeWindow, &(window->Data));
    
    //set GLFW event callbacks
    glfwSetWindowCloseCallback(window->NativeWindow, WindowCloseFunction);
    glfwSetWindowSizeCallback(window->NativeWindow, WindowResizeFunction);    
    glfwSetKeyCallback(window->NativeWindow, WindowKeyFunction);    
    glfwSetCharCallback(window->NativeWindow, WindowCharFunction);    
    glfwSetMouseButtonCallback(window->NativeWindow, WindowMouseButtonFunction);
    glfwSetScrollCallback(window->NativeWindow, WindowMouseScrollFunction);

    return window;
}

void lc_UpdateWindow(lc_Window* window)
{
    glfwPollEvents();
    if(window->Data.Width > 0 && window->Data.Height > 0) //don't swap buffers when the window is minimised
        glfwSwapBuffers(window->NativeWindow);
}

void lc_SetWindowVSync(lc_Window* window, bool enabled)
{
    window->Data.VSync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

void lc_DestroyWindow(lc_Window* window)
{
    glfwDestroyWindow(window->NativeWindow);
    free(window);
}
