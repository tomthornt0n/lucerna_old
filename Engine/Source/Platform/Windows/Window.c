/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 27 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

u8 _lcKeyLUT[256] = 
{

    0, 41, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 42, 43, 20, 26, 8,
    21, 23, 28, 24, 12, 18, 19, 47, 48, 158, 224, 4, 22, 7, 9, 10, 11, 13, 14,
    15, 51, 52, 53, 225, 49, 29, 27, 6, 25, 5, 17, 16, 54, 55, 56, 229, 0, 226,
    0, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 72, 71, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 228, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 70, 230, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74,
    82, 75, 0, 80, 0, 79, 0, 77, 81, 78, 73, 76, 0, 0, 0, 0, 0, 0, 0, 227, 231,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct
{
    u32 Width, Height;

    b8 DummyContext;

    HWND WindowHandle;
    HDC DeviceContext;
    HGLRC RenderContext;
} _lcWindow;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

internal void
_lcWindowInit(HINSTANCE instanceHandle,
             i8 *title,
             u32 width, u32 height,
             b8 vSyncEnabled)
{
    WNDCLASS windowClass;
    PIXELFORMATDESCRIPTOR pfd;
    i32  pixelFormat;
    HMODULE opengl32;
    b8 recreateContext;

    _lcWindow.Width = width;
    _lcWindow.Height = height;

    _lcWindow.DummyContext = true;

    windowClass;
    ZeroMemory(&windowClass, sizeof(windowClass));

    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instanceHandle;
    windowClass.lpszClassName = "LUCERNA";

    RegisterClass(&windowClass);

    _lcWindow.WindowHandle = CreateWindow("LUCERNA",
                                          title,
                                          WS_OVERLAPPEDWINDOW,
                                          CW_USEDEFAULT, CW_USEDEFAULT,
                                          width, height,
                                          NULL,
                                          NULL,
                                          instanceHandle,
                                          NULL);

    _lcWindow.DeviceContext = GetDC(_lcWindow.WindowHandle);

    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW      |
                       PFD_SUPPORT_OPENGL      |
                       PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
    pfd.cDepthBits   = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType   = PFD_MAIN_PLANE;

    pixelFormat = ChoosePixelFormat(_lcWindow.DeviceContext, &pfd);
    SetPixelFormat(_lcWindow.DeviceContext, pixelFormat, &pfd);

    _lcWindow.RenderContext = wglCreateContext(_lcWindow.DeviceContext);
    wglMakeCurrent(_lcWindow.DeviceContext, _lcWindow.RenderContext);

    opengl32 = LoadLibrary("opengl32.dll");

    gl.wglGetExtensionsStringARB =
        (PFNWGLGETEXTENSIONSSTRINGARBPROC)
        _lcGLLoadFunction("wglGetExtensionsStringARB", opengl32);

    recreateContext = _lcGLIsExtensionSupported(_lcWindow.DeviceContext,
                                               "WGL_ARB_pixel_format") &&
                      _lcGLIsExtensionSupported(_lcWindow.DeviceContext,
                                               "WGL_ARB_create_context");

    if (_lcGLIsExtensionSupported(_lcWindow.DeviceContext,
                                 "WGL_EXT_swap_control"))
    {
        gl.wglSwapIntervalEXT = _lcGLLoadFunction("wglSwapIntervalEXT",
                                                  opengl32);
    }
    else
    {
        LC_CORE_LOG_WARN("'WGL_EXT_swap_control' not supported.");
        gl.wglSwapIntervalEXT = NULL;
    }

    gl.wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)
        _lcGLLoadFunction("wglChoosePixelFormatARB", opengl32);

    gl.wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        _lcGLLoadFunction("wglCreateContextAttribsARB", opengl32);

    FreeModule(opengl32);

    if (recreateContext)
    {
        i32 pixelFormatARB;
        i32 pixelAttributes[] = 
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0
        };
        UINT pixelFormatCount;
        BOOL ChoosePixelFormatResult;
        GLint contextAttributes[] = 
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(_lcWindow.RenderContext);
        ReleaseDC(_lcWindow.WindowHandle, _lcWindow.DeviceContext);
        DestroyWindow(_lcWindow.WindowHandle);

        _lcWindow.WindowHandle = CreateWindow("LUCERNA",
                                              title,
                                              WS_OVERLAPPEDWINDOW,
                                              CW_USEDEFAULT, CW_USEDEFAULT,
                                              width, height,
                                              NULL,
                                              NULL,
                                              instanceHandle,
                                              NULL);

        _lcWindow.DeviceContext = GetDC(_lcWindow.WindowHandle);


        ChoosePixelFormatResult =
            gl.wglChoosePixelFormatARB(_lcWindow.DeviceContext,
                                       pixelAttributes,
                                       NULL,
                                       1,
                                       &pixelFormatARB,
                                       &pixelFormatCount);

        LC_ASSERT(ChoosePixelFormatResult == TRUE &&
                  pixelFormatCount > 0,
                  "Error creating window: could not find pixel format");

        SetPixelFormat(_lcWindow.DeviceContext, pixelFormatARB, &pfd);


        _lcWindow.RenderContext = gl.wglCreateContextAttribsARB(_lcWindow.DeviceContext,
                                                                0,
                                                                contextAttributes);

        wglMakeCurrent(NULL, NULL);
        wglMakeCurrent(_lcWindow.DeviceContext, _lcWindow.RenderContext);
    }

    lcWindowSetVSync(vSyncEnabled);

    ShowWindow(_lcWindow.WindowHandle, SW_SHOW);
}

void
lcWindowUpdate(void)
{
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    SwapBuffers(_lcWindow.DeviceContext);
}

LRESULT CALLBACK
WindowProc(HWND windowHandle,
           UINT message,
           WPARAM wParam,
           LPARAM lParam)
{
    static i32 mouseScrollPrev = 0; /* NOTE(tbt): keep track of previous
                                                  frame's scroll so mouse wheel
                                                  movements can be treated as
                                                  mouse button presses (e.g.
                                                  with a release event, etc.) */

    lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_5] = false;
    lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_6] = false;
    switch(mouseScrollPrev)
    {
        case 1:
        {
            mouseScrollPrev = 0;
            _lcMessageEmit(_lcMouseButtonReleaseMessageCreate(LC_MOUSE_BUTTON_5));
        }
        case -1:
        {
            mouseScrollPrev = 0;
            _lcMessageEmit(_lcMouseButtonReleaseMessageCreate(LC_MOUSE_BUTTON_6));
        }
    }

    switch(message)
    {
        case WM_KEYDOWN:
        {
            u32 key;
            key =  _lcKeyLUT[((lParam >> 16) & 0x7f) |
                             ((lParam & (1 << 24))
                             != 0 ?
                             0x80 : 0)];

            _lcMessageEmit(_lcKeyPressMessageCreate(key));
            lcInputIsKeyPressed[key] = true;

            break;
        }
        case WM_KEYUP:
        {
            u32 key;
            key =  _lcKeyLUT[((lParam >> 16) & 0x7f) |
                             ((lParam & (1 << 24))
                             != 0 ?
                             0x80 : 0)];

            _lcMessageEmit(_lcKeyReleaseMessageCreate(key));
            lcInputIsKeyPressed[key] = false;

            break;
        }
        case WM_LBUTTONDOWN:
        {
            _lcMessageEmit(_lcMouseButtonPressMessageCreate(LC_MOUSE_BUTTON_1));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_1] = true;

            break;
        }
        case WM_LBUTTONUP:
        {
            _lcMessageEmit(_lcMouseButtonReleaseMessageCreate(LC_MOUSE_BUTTON_1));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_1] = false;

            break;
        }
        case WM_MBUTTONDOWN:
        {
            _lcMessageEmit(_lcMouseButtonPressMessageCreate(LC_MOUSE_BUTTON_2));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_2] = true;

            break;
        }
        case WM_MBUTTONUP:
        {
            _lcMessageEmit(_lcMouseButtonReleaseMessageCreate(LC_MOUSE_BUTTON_2));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_2] = false;

            break;
        }
        case WM_RBUTTONDOWN:
        {
            _lcMessageEmit(_lcMouseButtonPressMessageCreate(LC_MOUSE_BUTTON_3));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_3] = true;

            break;
        }
        case WM_RBUTTONUP:
        {
            _lcMessageEmit(_lcMouseButtonReleaseMessageCreate(LC_MOUSE_BUTTON_3));
            lcInputIsMouseButtonPressed[LC_MOUSE_BUTTON_3] = false;

            break;
        }
        case WM_MOUSEWHEEL:
        {
            i32 offset;
            i32 button;

            offset = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
            _lcMessageEmit(_lcMouseScrollMessageCreate(offset));

            button = offset > 0 ? LC_MOUSE_BUTTON_5 : LC_MOUSE_BUTTON_6;
            _lcMessageEmit(_lcMouseButtonPressMessageCreate(button));
            lcInputIsMouseButtonPressed[button] = true;

            mouseScrollPrev = offset;
        }
        case WM_MOUSEMOVE:
        {
            lcInputMousePosition[0] = GET_X_LPARAM(lParam);
            lcInputMousePosition[1] = GET_Y_LPARAM(lParam);

            break;
        }
        case WM_SIZE:
        {

            _lcWindow.Width = LOWORD(lParam);
            _lcWindow.Height = HIWORD(lParam);
            _lcMessageEmit(_lcWindowResizeMessageCreate(_lcWindow.Width,
                                                        _lcWindow.Height));
            break;
        }
        case WM_DESTROY:
        {
            if (_lcWindow.DummyContext)
            {
                _lcWindow.DummyContext = false;
            }
            else
            {
                _lcMessageEmit(_lcWindowCloseMessageCreate());
                PostQuitMessage(0);
            }
            break;
        }
        default:
        {
            return DefWindowProc(windowHandle, message, wParam, lParam);
        }
    }

    return 0;
}

void
lcWindowSetVSync(b8 enabled)
{
    if (gl.wglSwapIntervalEXT)
    {
        gl.wglSwapIntervalEXT(enabled);
    }
}

void
lcWindowGetSize(u32 *result)
{
    result[0] = _lcWindow.Width;
    result[1] = _lcWindow.Height;
}

internal void
_lcWindowDestroy(void)
{
    wglMakeCurrent(_lcWindow.DeviceContext, NULL);
    wglDeleteContext(_lcWindow.RenderContext);
    ReleaseDC(_lcWindow.WindowHandle, _lcWindow.DeviceContext);
    DestroyWindow(_lcWindow.WindowHandle);
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

