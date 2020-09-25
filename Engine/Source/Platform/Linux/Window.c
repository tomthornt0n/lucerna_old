/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 Sep 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


uint8_t KeyLUT[256] =
{
    0, 41, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 42, 43, 20, 26, 8,
    21, 23, 28, 24, 12, 18, 19, 47, 48, 158, 224, 4, 22, 7, 9, 10, 11, 13, 14,
    15, 51, 52, 53, 225, 49, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 229, 85,
    226, 44, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 83, 71, 95, 96, 97,
    86, 92, 93, 94, 87, 89, 90, 91, 98, 99, 0, 0, 100, 68, 69, 0, 0, 0, 0, 0,
    0, 0, 88, 228, 84, 154, 230, 0, 74, 82, 75, 80, 79, 77, 81, 78, 73, 76, 0,
    0, 0, 0, 0, 103, 0, 72, 0, 0, 0, 0, 0, 227, 231, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0
};

static int VisualAttributes[] =
{
    GLX_X_RENDERABLE, True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    GLX_STENCIL_SIZE, 8,
    GLX_DOUBLEBUFFER, True,
    None
};

struct
{
    uint32_t Width, Height;

    Display *Display;
    int DefaultScreenID;
    xcb_connection_t *Connection; 
    xcb_window_t NativeWindow;
    GLXContext Context;
    GLXDrawable Drawable;
    xcb_atom_t WindowClose;
} lcWindow;

static void
lcWindowInit(char *title,
             uint32_t width, uint32_t height,
             bool vSyncEnabled)
{

    lcWindow.Display = XOpenDisplay(0);
    LC_ASSERT(lcWindow.Display, "Could not open X display");

    lcWindow.DefaultScreenID = DefaultScreen(lcWindow.Display);

    lcWindow.Connection = XGetXCBConnection(lcWindow.Display);

    if(!lcWindow.Connection)
    {
        XCloseDisplay(lcWindow.Display);
        LC_CORE_LOG_ERROR("Can't get XCB connection from display");
        exit(-1);
    }

    XSetEventQueueOwner(lcWindow.Display, XCBOwnsEventQueue);

    xcb_screen_t *screen = 0;

    const xcb_setup_t *setup = xcb_get_setup(lcWindow.Connection);
    xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);

    int i;
    for(i = lcWindow.DefaultScreenID;
        screenIterator.rem && i > 0;
        --i, xcb_screen_next(&screenIterator));

    screen = screenIterator.data;
    
    int visualID = 0;

    GLXFBConfig *frameBufferConfigs = NULL;
    int frameBufferConfigCount = 0;

    frameBufferConfigs = gl.XChooseFBConfig(lcWindow.Display,
                                           lcWindow.DefaultScreenID,
                                           VisualAttributes,
                                           &frameBufferConfigCount);

    LC_ASSERT(frameBufferConfigs && frameBufferConfigCount != 0,
              "Error getting frame buffer configs");

    GLXFBConfig frameBufferConfig = frameBufferConfigs[0];
    gl.XGetFBConfigAttrib(lcWindow.Display, frameBufferConfig, GLX_VISUAL_ID, &visualID);

    lcWindow.Context = gl.XCreateNewContext(lcWindow.Display,
                                           frameBufferConfig,
                                           GLX_RGBA_TYPE,
                                           0,
                                           True);

    LC_ASSERT(lcWindow.Context, "OpenGL context creation failed");

    xcb_colormap_t colourMap = xcb_generate_id(lcWindow.Connection);
    lcWindow.NativeWindow = xcb_generate_id(lcWindow.Connection);

    xcb_create_colormap(lcWindow.Connection,
                        XCB_COLORMAP_ALLOC_NONE,
                        colourMap,
                        screen->root,
                        visualID);

    uint32_t eventMask = XCB_EVENT_MASK_EXPOSURE         |
                         XCB_EVENT_MASK_KEY_PRESS        |
                         XCB_EVENT_MASK_KEY_RELEASE      |
                         XCB_EVENT_MASK_BUTTON_PRESS     |
                         XCB_EVENT_MASK_BUTTON_RELEASE   |
                         XCB_EVENT_MASK_POINTER_MOTION   |
                         XCB_EVENT_MASK_ENTER_WINDOW     |
                         XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    uint32_t valueList[] = { eventMask, colourMap, 0 };
    uint32_t valueMask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;

    xcb_create_window(lcWindow.Connection,
                      XCB_COPY_FROM_PARENT,
                      lcWindow.NativeWindow,
                      screen->root,
                      0, 0,
                      width, height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      visualID,
                      valueMask,
                      valueList);

    xcb_intern_atom_cookie_t
    wmProtocolCookie = xcb_intern_atom(lcWindow.Connection,
                                       1,
                                       12,
                                       "WM_PROTOCOLS");

    xcb_intern_atom_reply_t *
    wmProtocolReply = xcb_intern_atom_reply(lcWindow.Connection,
                                            wmProtocolCookie,
                                            0);

    xcb_intern_atom_cookie_t
    windowCloseCookie = xcb_intern_atom(lcWindow.Connection,
                                        0,
                                        16,
                                        "WM_DELETE_WINDOW");

    lcWindow.WindowClose = xcb_intern_atom_reply(lcWindow.Connection,
                                                 windowCloseCookie,
                                                 0)->atom;

    xcb_change_property(lcWindow.Connection,    /* Connection                 */
                        XCB_PROP_MODE_REPLACE,  /* Mode                       */
                        lcWindow.NativeWindow,  /* Window                     */
                        wmProtocolReply->atom,  /* Property to change         */
                        4,                      /* Property type              */
                        32,                     /* Size of list elements      */
                        1,                      /* Number of elements in list */
                        &lcWindow.WindowClose); /* Property data              */

    xcb_change_property(lcWindow.Connection,    /* Connection                 */
                        XCB_PROP_MODE_REPLACE,  /* Mode                       */
                        lcWindow.NativeWindow,  /* Window                     */
                        XCB_ATOM_WM_NAME,       /* Property to change         */
                        XCB_ATOM_STRING,        /* Property type              */
                        8,                      /* Size of list elements      */
                        strlen(title),          /* Number of elements in list */
                        title);                 /* Property data              */

    xcb_map_window(lcWindow.Connection, lcWindow.NativeWindow);


    lcWindow.Drawable = gl.XCreateWindow(lcWindow.Display,
                                        frameBufferConfig,
                                        lcWindow.NativeWindow,
                                        NULL);

    if(!lcWindow.NativeWindow ||
       !lcWindow.Drawable)
    {
        xcb_destroy_window(lcWindow.Connection, lcWindow.NativeWindow);
        gl.XDestroyContext(lcWindow.Display, lcWindow.Context);

        LC_CORE_LOG_ERROR("Error creating OpenGL context");
        exit(-1);
    }

    if(!gl.XMakeContextCurrent(lcWindow.Display,
                              lcWindow.Drawable,
                              lcWindow.Drawable,
                              lcWindow.Context))
    {
        xcb_destroy_window(lcWindow.Connection, lcWindow.NativeWindow);
        gl.XDestroyContext(lcWindow.Display, lcWindow.Context);

        LC_CORE_LOG_ERROR("Could not make context current");
        exit(-1);
    }


    lcWindowSetVSync(vSyncEnabled);
}

void
lcWindowUpdate(void)
{
    xcb_generic_event_t *event = xcb_poll_for_event(lcWindow.Connection);

    if (event)
    {
        switch(event->response_type & ~0x80)
        {
            case XCB_KEY_PRESS:
            {
                xcb_key_press_event_t *press = 
                    (xcb_key_press_event_t *)event;

                /* HACK(tbt): assume evdev driver so subtract offset of 8 */
                uint8_t key = KeyLUT[press->detail - 8];

                lcMessageEmit(lcKeyPressMessageCreate(key));
                lcInputIsKeyPressed[key] = true;

                break;
            }
            case XCB_KEY_RELEASE:
            {
                /* HACK(tbt): assume evdev driver so subtract offset of 8 */
                uint32_t key = ((xcb_key_press_event_t *)event)->detail - 8;
                uint8_t lcKeyCode = KeyLUT[key];

                lcMessageEmit(lcKeyReleaseMessageCreate(lcKeyCode));
                lcInputIsKeyPressed[lcKeyCode] = false;

                break;
            }
            case XCB_BUTTON_PRESS:
            {
                uint8_t code = ((xcb_button_press_event_t *)event)->detail - 1;

                if (code == 3)
                {
                    lcMessageEmit(lcMouseScrollMessageCreate(1));
                }
                else if (code == 4)
                {
                    lcMessageEmit(lcMouseScrollMessageCreate(-1));
                }

                lcMessageEmit(lcMouseButtonPressMessageCreate(code));
                lcInputIsMouseButtonPressed[code] = true;

                break;
            }
            case XCB_BUTTON_RELEASE:
            {
                uint8_t code = ((xcb_button_press_event_t *)event)->detail - 1;

                lcMessageEmit(lcMouseButtonReleaseMessageCreate(code));
                lcInputIsMouseButtonPressed[code] = false;

                break;
            }
            case XCB_ENTER_NOTIFY:
            {
                xcb_enter_notify_event_t *enter =
                    (xcb_enter_notify_event_t *)event;

                lcInputMousePosition[0] = enter->event_x;
                lcInputMousePosition[1] = enter->event_y;

                break;
            }
            case XCB_MOTION_NOTIFY:
            {
                xcb_motion_notify_event_t *motion =
                    (xcb_motion_notify_event_t *)event;

                lcInputMousePosition[0] = motion->event_x;
                lcInputMousePosition[1] = motion->event_y;

                break;
            }
            case XCB_CONFIGURE_NOTIFY:
            {
                xcb_configure_notify_event_t *config =
                    (xcb_configure_notify_event_t *)event;

                if (lcWindow.Width != config->width ||
                    lcWindow.Height != config->height)
                {
                    lcWindow.Width = config->width;
                    lcWindow.Height = config->height;

                    lcMessageEmit(lcWindowResizeMessageCreate(lcWindow.Width,
                                                              lcWindow.Height));
                }

                break;
            }
            case XCB_CLIENT_MESSAGE:
            {
                if(((xcb_client_message_event_t*)event)->data.data32[0] ==
                   lcWindow.WindowClose)
                {
                    lcMessageEmit(lcWindowCloseMessageCreate());
                }

                break;
            }
            default:
                break;
        }
        free(event);
    }

    gl.XSwapBuffers(lcWindow.Display, lcWindow.Drawable);
}

void
lcWindowSetVSync(bool enabled)
{
    if (lcGLIsExtensionSupported(lcWindow.Display,
                                 lcWindow.DefaultScreenID,
                                 "GLX_EXT_swap_control"))
    {
        gl.XSwapIntervalEXT(lcWindow.Display,
                            lcWindow.Drawable,
                            enabled);
    }
    else if (lcGLIsExtensionSupported(lcWindow.Display,
                                      lcWindow.DefaultScreenID,
                                      "GLX_MESA_swap_control"))
    {
        gl.XSwapIntervalMESA(enabled);
    }
    else if (lcGLIsExtensionSupported(lcWindow.Display,
                                      lcWindow.DefaultScreenID,
                                      "GLX_SGI_swap_control"))
    {
        gl.XSwapIntervalSGI(enabled);
    }
}

void
lcWindowGetSize(uint32_t *result)
{
    result[0] = lcWindow.Width;
    result[1] = lcWindow.Height;
}

static void
lcWindowDestroy(void)
{
    gl.XDestroyWindow(lcWindow.Display, lcWindow.Drawable);
    xcb_destroy_window(lcWindow.Connection, lcWindow.NativeWindow);
    gl.XDestroyContext(lcWindow.Display, lcWindow.Context);
    XCloseDisplay(lcWindow.Display);
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

