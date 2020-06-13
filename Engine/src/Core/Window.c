/* stored in the GLFWwindow's user pointer area, so can be acessed from the GLFWwindow, without need of the lcWindow_t */
typedef struct
{
    const char *Title;
    int Width, Height;
    uint8_t VSync;

    /* event callbacks */
    void (*WindowClosedCallback)();
    void (*WindowResizeCallback)(int width, int height);
    void (*KeyPressedCallback)(int keyCode, uint8_t repeat);
    void (*KeyReleasedCallback)(int keycode);
    void (*KeyTypedCallback)(int keyCode);
    void (*MouseButtonPressedCallback)(int keyCode);
    void (*MouseButtonReleasedCallback)(int keyCode);
    void (*MouseScrolledCallback)(double xOffset, double yOffset);
} lc_WindowData;

typedef struct
{
    GLFWwindow *NativeWindow;
    lc_WindowData Data; 
} lcWindow_t;


static uint8_t s_GLFWInitialized = 0;

static void GLFWErrorCallback(int error, const char *description)
{
    LC_ASSERT(0, "GLFW ERROR(%d): %s", error, description);
}

/* default user event callbacks */
static void DefaultWindowCloseCallback(void) {}
static void DefaultWindowResizeCallback(int width, int height) {}
static void DefaultKeyPressedCallback(int keyCode, uint8_t repeat) {}
static void DefaultKeyReleasedCallback(int keycode) {}
static void DefaultKeyTypedCallback(int keyCode) {}
static void DefaultMouseButtonPressedCallback(int keyCode) {}
static void DefaultMouseButtonReleasedCallback(int keyCode) {}
static void DefaultMouseScrolledCallback(double xOffset, double yOffset) {}

/* window event callback functions */
static void WindowCloseFunction(GLFWwindow *window)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->WindowClosedCallback();
};
static void WindowResizeFunction(GLFWwindow *window,
                                 int width, int height)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->Width = width;
    data->Height = height;

    data->WindowResizeCallback(width, height);
};
static void WindowKeyFunction(GLFWwindow *window,
                              int key, int scanCode, int action, int mods)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    switch (action)
    {
        case GLFW_PRESS:
        {
            data->KeyPressedCallback(key, 0);
            break;
        }
        case GLFW_RELEASE:
        {
            data->KeyReleasedCallback(key);
            break;
        }
        case GLFW_REPEAT:
        {
            data->KeyPressedCallback(key, 1);
            break;
        }
    }
};
static void WindowCharFunction(GLFWwindow *window, unsigned int key)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->KeyTypedCallback(key);
};
static void WindowMouseButtonFunction(GLFWwindow *window,
                                      int key, int action, int mods)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
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
static void WindowMouseScrollFunction(GLFWwindow *window,
                                      double xOffset, double yOffset)
{
    lc_WindowData *data = (lc_WindowData*)glfwGetWindowUserPointer(window);
    data->MouseScrolledCallback(xOffset, yOffset);
};

void lc_CreateWindow(lcWindow_t* output,
                     const char *title,
                     int width, int height)
{
    output->Data.Title = title;
    output->Data.Width = width;
    output->Data.Height = height;

    output->Data.WindowClosedCallback = DefaultWindowCloseCallback;
    output->Data.WindowResizeCallback = DefaultWindowResizeCallback;
    output->Data.KeyPressedCallback = DefaultKeyPressedCallback;
    output->Data.KeyReleasedCallback = DefaultKeyReleasedCallback;
    output->Data.KeyTypedCallback = DefaultKeyTypedCallback;
    output->Data.MouseButtonPressedCallback = DefaultMouseButtonPressedCallback;
    output->Data.MouseButtonReleasedCallback = DefaultMouseButtonReleasedCallback;
    output->Data.MouseScrolledCallback = DefaultMouseScrolledCallback;


    if (!s_GLFWInitialized)
    {
        int success = glfwInit();
        LC_ASSERT(success, "Could not initialize GLFW");

        glfwSetErrorCallback(GLFWErrorCallback);

        s_GLFWInitialized = 1;
    }

    output->NativeWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    LC_ASSERT(output->NativeWindow, "GLFW ERROR: Window creation failed!");
    
    glfwMakeContextCurrent(output->NativeWindow);

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LC_ASSERT(status, "ERROR: failed to initialise glad!");

    glfwSetWindowUserPointer(output->NativeWindow, &(output->Data));
    
    /* set GLFW event callbacks */
    glfwSetWindowCloseCallback(output->NativeWindow, WindowCloseFunction);
    glfwSetWindowSizeCallback(output->NativeWindow, WindowResizeFunction);    
    glfwSetKeyCallback(output->NativeWindow, WindowKeyFunction);    
    glfwSetCharCallback(output->NativeWindow, WindowCharFunction);    
    glfwSetMouseButtonCallback(output->NativeWindow, WindowMouseButtonFunction);
    glfwSetScrollCallback(output->NativeWindow, WindowMouseScrollFunction);
}

void lc_UpdateWindow(lcWindow_t *window)
{
    glfwPollEvents();

    /* don't swap buffers when the window is minimised */
    if(window->Data.Width > 0 && window->Data.Height > 0)
        glfwSwapBuffers(window->NativeWindow);
}

void lc_SetWindowVSync(lcWindow_t *window, uint8_t enabled)
{
    window->Data.VSync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

void lc_DestroyWindow(lcWindow_t *window)
{
    glfwDestroyWindow(window->NativeWindow);
}
