/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 28 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef LC_PLATFORM_LINUX

i32
main(i32 argc,
     i8 **argv)
{
    lcInitConfig_t config;

    config = lcClientConfig();

    _lcGLLoad();
    _lcInputInit();
    _lcMessageSystemInit();
    _lcWindowInit(config.WindowTitle,
                 config.WindowDimensions[0],
                 config.WindowDimensions[1],
                 config.VSyncEnabled);
    _lcRendererInit();
    _lcLoadMasterTexture();
    _lcCameraInit(config.CameraPosition);

    lcClientMain(argc, argv);

    _lcRendererDestroy();
    _lcCameraDestroy();
    _lcWindowDestroy();
    _lcMessageSystemDestroy();

    return 0;
}

#elif defined LC_PLATFORM_WINDOWS

i32
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        i32 nShowCmd)
{
    lcInitConfig_t config;

    lcClockInit();

    config = lcClientConfig();

#ifdef LC_LOGGING_ENABLED
    lcLogInitWindowsConsole();
#endif
    lcInputInit();
    lcMessageSystemInit();
    lcWindowInit(hInstance,
                 config.WindowTitle,
                 config.WindowDimensions[0],
                 config.WindowDimensions[1],
                 config.VSyncEnabled);
    lcGLLoad();
    lcRendererInit();
    lcLoadMasterTexture();
    lcCameraInit(config.CameraPosition);

    lcClientMain(__argc, __argv);

    lcRendererDestroy();
    lcCameraDestroy();
    lcMessageSystemDestroy();

    return 0;
}

i32
main(void)
{
    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
#else
#error No platform macro defined
#endif

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

